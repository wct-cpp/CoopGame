// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerupActor;

UCLASS()
class COOPGAME_API ASPickActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,Category="Components")
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere,Category="Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditAnywhere,Category="PickupActor")
	TSubclassOf<ASPowerupActor> PowerupClass;

	ASPowerupActor* PowerupInstance;

	UPROPERTY(EditAnywhere,Category="PickupActor")
	float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	void Respawn();

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
