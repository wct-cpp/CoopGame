// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "CoopGame/Public/SProjectile.h"
#include "SProjectileWeapon.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()
	
protected:
	void virtual Fire() override;

	APawn* Pawn;


public:
	ASProjectileWeapon();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASProjectile> ProjectileClass=ASProjectile::StaticClass();//需要为ProjectileClass分配一个有效的类或子类


};
