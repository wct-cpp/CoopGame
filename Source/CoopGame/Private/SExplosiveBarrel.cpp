// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	RadialForceComp=CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(RootComponent);
	RadialForceComp->Radius = 0;
	RadialForceComp->ForceStrength = 0;
	RadialForceComp->bImpulseVelChange = true;//设置效果是否与物理相关，比如是否考虑质量
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);//设置作用对象的碰撞类型
	RadialForceComp->bIgnoreOwningActor = false;//是否考虑将力作用到组件的拥有者

	SetReplicates(true);
	SetReplicateMovement(true);//复制actor的移动
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExploded) {
		return;
	}
	if (Health <= 0 && !bExploded) {
		bExploded = true;
		//Explosive!
		Explosive();
	}
}

void ASExplosiveBarrel::Explosive()
{
	//若是客户端，则让服务端执行函数
	if (!HasAuthority()) { 
		ServerExplosive();
	}

	RadialForceComp->Radius = ExplosiveRadius;
	RadialForceComp->ForceStrength = ExplosiveImpulse;
	RadialForceComp->FireImpulse();

	OnRep_Exploded();
}

void ASExplosiveBarrel::ServerExplosive_Implementation()
{
	Explosive();
}

bool ASExplosiveBarrel::ServerExplosive_Validate()
{
	return true;
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(5.0f));

	MeshComp->AddImpulse(FVector::UpVector * ExplosiveImpulse, NAME_None, true);
	MeshComp->SetMaterial(0, ExplodedMaterial);
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bExploded);
}