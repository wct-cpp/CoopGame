// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly,Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly,Category="Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly,Category="Components")
	USphereComponent* ShpereComp;

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float RequireDistanceToTarget;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwingHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//受到伤害后脉冲闪烁以及提升能量等级的的动态材质
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	UParticleSystem* DestructFX;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float SelfDamageInterval;

	UPROPERTY(ReplicatedUsing=OnRep_SelfDestruct)
	bool bExploded;

	UFUNCTION()
	void OnRep_SelfDestruct();

	FTimerHandle TimerHandle_SelfDamage;


	bool bStartedSelfDestruction;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	USoundCue* ExplodeSound;

	UFUNCTION(NetMulticast,Unreliable)
	void ChangeState();
	
	int32 PowerLevel;

	void OnCheckNearbyBots();

	FTimerHandle TimeHandle_RefreshPath;

	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
