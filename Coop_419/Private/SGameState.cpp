// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "Net/UnrealNetwork.h"




void ASGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveState_Changed(WaveState, OldState);
}


void ASGameState::SetWaveState(EWaveState NewState)
{
	if (Role == ROLE_Authority)
	{

		EWaveState OldState = WaveState;

		WaveState = NewState;
		//call on Server, we need to call this manually for server, because it´s only caleld on the client, otherwise WaveState_Changed is never called
		OnRep_WaveState(OldState);
	}
}

//we need this when dealing with Multiplayer Games otehrwise the compiler complains
void ASGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
}