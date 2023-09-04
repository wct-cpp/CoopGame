// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "CoopGame/Public/SCharacter.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	ShpereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	ShpereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ShpereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	ShpereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ShpereComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ShpereComp->SetupAttachment(MeshComp);

	bUseVelocityChange = true;
	MovementForce = 1000;
	RequireDistanceToTarget = 100;

	bExploded = false;

	bStartedSelfDestruction = false;

	SelfDamageInterval = 0.25f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	ShpereComp->SetSphereRadius(ExplosionRadius);

	if (HasAuthority()) {
		//Find initial move to
		NextPathPoint = GetNextPathPoint();
	}

	FTimerHandle TimerHandle_CheckPowerLevel;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ASTrackerBot::OnCheckNearbyBots, 1.0f, true);
}

FVector ASTrackerBot::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance=FLT_MAX;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It) {
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || USHealthComponent::IsFriendly(BestTarget,this)) {
			continue;
		}

		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f) {
			float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
			if (Distance < NearestTargetDistance) {
				BestTarget = TestPawn;
				Distance = NearestTargetDistance;
			}
		}
	}

	if (BestTarget) {
		UNavigationPath* NavPath=UNavigationSystemV1::FindPathToActorSynchronously(this,GetActorLocation(),BestTarget);

		GetWorldTimerManager().ClearTimer(TimeHandle_RefreshPath);

		if (NavPath&&NavPath->PathPoints.Num() > 1) {

			GetWorldTimerManager().SetTimer(TimeHandle_RefreshPath, this,&ASTrackerBot::RefreshPath, 5.0f, false);//防卡住

			//Return next point in the path
			return NavPath->PathPoints[1];
		}
	}


	//Failed to find path
	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwingHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{	 
	ChangeState();

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health),*GetName());

	//Explode on hitpoints==0
	if (Health <= 0 ) {
		SelfDestruct();
	}
}

void ASTrackerBot::SelfDestruct()
{	
	if (bExploded) {
		return;
	}

	bExploded = true;

	OnRep_SelfDestruct();

	if (HasAuthority()) {
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		//Increase damage based on the power level
		float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);

		UGameplayStatics::ApplyRadialDamage(GetWorld(), ActualDamage, GetActorLocation(), ExplosionRadius, nullptr,IgnoredActors ,this,GetInstigatorController(),true);//造成辐射伤害

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 20, FColor::Orange, false, 3.0f,0,1.0f);

		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::OnRep_SelfDestruct()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructFX, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound,GetActorLocation());//爆炸音效

	MeshComp->SetVisibility(false,true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}



void ASTrackerBot::ChangeState_Implementation()
{
	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst) {
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
}

void ASTrackerBot::OnCheckNearbyBots()
{
	const float Radius = ExplosionRadius;

	//Create temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	//Only find pawns
	FCollisionObjectQueryParams QueryParams;
	//Our tracker bot's mesh component is set to Physics Body in Buleprint(default profile of physics simulated actors)
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FColor::Blue, false, 1.0f);

	int32 NrOfBots=0;
	//loop over the results using a "range based for loop"
	for (FOverlapResult Result : Overlaps) {
		//Check if we overlapped with another tracker bot(ignoring player and other bot types)
		ASTrackerBot* Bot = Cast<ASTrackerBot>(Result.GetActor());
		//Ignore this trackerbot instance
		if (Bot && Bot != this) {
			NrOfBots++;
		}
	}

	const int32 MaxPowerLevel = 4;

	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);

	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst) {
		float Alpha = PowerLevel / (float)MaxPowerLevel;

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 0), FString::FromInt(PowerLevel), this, FColor::White, 1.0f, true);
}

void ASTrackerBot::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()&&!bExploded) {
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget<=RequireDistanceToTarget) {

			NextPathPoint = GetNextPathPoint();

			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
		}
		else {
			//Keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 100, FColor::Red, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 4.f, 1.0f);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction&&!bExploded) {
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn && !USHealthComponent::IsFriendly(this,OtherActor)) {
			//We overlapped with a player!
				
			bStartedSelfDestruction = true;

			if (HasAuthority()) {
				//开始自毁
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0);
			}
	
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}


void ASTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASTrackerBot, bExploded,COND_SkipOwner);
}
