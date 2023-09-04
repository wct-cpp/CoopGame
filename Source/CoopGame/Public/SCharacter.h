// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();


	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="Components")//����ͨ���ڹ��캯���д���ʵ�������������Ϊ���ԣ�����ֱ������һ��ȫ�µĶ���
	UCameraComponent* CameraComp;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="Components")
	USHealthComponent* HealthComp;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	float ZoomedFOV;

	/*Default FOV set during begin play*/
	float DefaultFOV;

	void BeginZoom();

	void EndZoom();

	UPROPERTY(EditDefaultsOnly,Category="Player",meta=(ClampMin=0.1,ClampMax=100))
	float ZoomInterpSpeed;

	UPROPERTY(Replicated)//�ͻ��˸���
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly,Category="Player")
	FName WeaponAttachSocketName;	



	UFUNCTION()
	void OnHealthChanged_C(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/* Pawn died previously*/
	UPROPERTY(Replicated,BlueprintReadOnly,Category="Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const;

	UFUNCTION(BlueprintCallable,Category="Player")
	void StartFire();
	UFUNCTION(BlueprintCallable,Category="Player")
	void StopFire();
};
