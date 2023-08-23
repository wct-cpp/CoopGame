// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"


ASProjectileWeapon::ASProjectileWeapon() {
	Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
}

void ASProjectileWeapon::Fire(){

	AActor* MyOwner = GetOwner();
	if (MyOwner && ProjectileClass) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);//这个函数将眼睛的位置和角度传给eyeL/R 参数

		FVector MuzzleLocation = MeshComp->GetSocketLocation("MuzzleSocket");

		FRotator MuzzleRotation = Pawn->GetControlRotation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.Instigator = Pawn;
		
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, ActorSpawnParams);
	}

	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
}
