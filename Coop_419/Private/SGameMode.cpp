// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "SGameState.h"
#include "SHealthComponent.h"
#include "TimerManager.h"
#include "SPlayerState.h"

enum class EWaveState:uint8;


ASGameMode::ASGameMode()
{
	WaveCount = 0;
	NrOfBotsToSpawn = 0;
	TimeBetweenWaves = 2.0f;

	//activate tick and reduce the frequency (alternative for making another Timer)
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	PlayerStateClass = ASPlayerState::StaticClass();

	GameStateClass = ASGameState::StaticClass();
}


void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();

	CheckAnyPlayerAlive();
}

void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}


void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::WaitingToStart);

	RestartDeadPlayers();
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		//There is still work to do
		return;
	}


	bool bIsAnyBotAlive = false;

	//Gets a list of all the Pawns in the level we can iterate on , check UnrealSource if we forget something
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			//we dont need to check any other pawn
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{

		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			//like a Breakpoint, if Test fails the Code stops here and it has something to be wrong in the code
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				 //A player is still alive
				return;
			} 
		}
	}

	//No Player is alive, end the game

	GameOver();
}

void ASGameMode::GameOver()
{

	EndWave();

	//@ToDo: Finish up the match, present GameOver to players
	UE_LOG(LogTemp, Log, TEXT("GAME OVER: All Player died."));

	SetWaveState(EWaveState::GameOver);
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	//Template see Source
	ASGameState* GS = GetGameState<ASGameState>();

	//if this doesn´t suceed we want to be notified
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);

	}
}

void ASGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PC = It->Get();
		//if GetPawn == lost == died
		if (PC && PC->GetPawn() == nullptr)
		{
			//FUnctionality from GM !
			RestartPlayer(PC);
		}
	}
}

void ASGameMode::SpawnBotTimerElapsed()
{
	//BP-Magic
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}
