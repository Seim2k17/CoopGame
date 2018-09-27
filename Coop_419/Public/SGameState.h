// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,

	WaveInProgress,

	//no  longer spawn new Bots, wait for players to kill remaining bots
	WaitingToComplete,

	WaveComplete,

	GameOver,
};


/**
 * 
 */
UCLASS()
class COOP_419_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()


protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveState_Changed(EWaveState NewState, EWaveState Old);

	//BP_ReadOnly because we want that BP not interferring but reading the Enum
	//ReplictaeUsing because we want to be able to react to this / lets us pass in whatever the previous state was / e.g. first unequip the old weapon before equip the new one ...
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:

	void SetWaveState(EWaveState NewState);
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

};
