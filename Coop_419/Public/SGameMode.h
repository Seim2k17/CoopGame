// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * we derived from GameModeBase since we don´t use all functionality from GameMode.h (to much) instead we derive from gameModeBase which does the minimum
 * 3 classes are very important for this project: A)GameMode B) GameState (replicated State and Variables for Multiplayer) C) PlayerState which holds all infos about the Player
 * GameModeClass only exist on the Server, so it´s fine for SinglePlayer but it´s never delivered to  the Clients in Multiplayer
 * to replicate some states to the clients we need GameState_Classes
 */

enum class EWaveState : uint8;

// KilledActor,KillerActor
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilledSignature, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

UCLASS()
class COOP_419_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilledSignature OnActorKilled;

/** Members */
protected:

	UPROPERTY()
	FTimerHandle TimerHandle_BotSpawner;

	UPROPERTY()
	FTimerHandle TimerHandle_NextWaveStart;

	// Bots to spawn in current wave
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BotSpawning")
	int32 NrOfBotsToSpawn;

	//incremented every new Wave
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

/** Methods */
public:
	
	ASGameMode();

	void StartPlay() override;

	void Tick(float DeltaSeconds) override;

protected:

	//do some functionality in c++ but all the stuff which is easier to do in BP we will add in Blueprint (running EQS...)
	//so first we decide where we place what functionality

	//Hook for BP to Spawn a single Bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	//Start Spawing Bots
	void StartWave();

	//stop Spawning Bots
	void EndWave();

	//Set Timer for next StartWave
	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();
	
};
