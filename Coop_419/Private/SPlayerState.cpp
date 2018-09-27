// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"




void ASPlayerState::AddScore(float ScoreDelta)
{
	//from UE4::PlayerState (but BPReadOnly)
	Score += ScoreDelta;
}
