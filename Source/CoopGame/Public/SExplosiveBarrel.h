// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class USHealthComponent;
class URadialForceComponent;
class UMaterialInterface;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	URadialForceComponent* RadialForceComp;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Explosive")
	UParticleSystem* ExplosiveEffect;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Explosive")
	UMaterialInterface* ExplodedMaterial;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Explosive")
	float ExplosiveImpulse;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Explosive")
	float ExplosiveRadius;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(ReplicatedUsing=OnRep_Exploded,BlueprintReadOnly,Category="Explosive")
	bool bExploded; 

	UFUNCTION()
	void Explosive();

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerExplosive();

	UFUNCTION()
	void OnRep_Exploded();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
