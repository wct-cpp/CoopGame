// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8 ;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled,AActor*, VictimActor, AActor*, KillerActor,AController*, KillerController);

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

protected:
	
	//Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent,Category="GameMode")
	void SpawnNewBot();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void GameOver();

	void SpawnBotTimerElapsed();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	//当前波次生成的Bot数量
	int32 NrOfBotToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly,Category="GameMode")
	float TimeBetweenWaves;



public:
	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable,Category="GameMode")
	FOnActorKilled OnActorKilled;
};
