// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UParticleSystem;

UCLASS()
class COOPGAME_API ASProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:


	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UParticleSystem* ExplosionFX;

	UPROPERTY(VisibleAnywhere, EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(VisibleAnywhere, EditDefaultsOnly, Category = "Damage")
	float DamageRadius;

	TSubclassOf<UDamageType> DamageType;

	TArray<AActor*>& IgnoreActors;

	UFUNCTION()
	void Explode();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintExplode();
};
