// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * It holds variables we want to replicate so that
 * a pawn might die and be respawned (not persist)
 * playercontroller is persistant but doesn´t properly replicated, so if we want to see other players score  we need this class
 * playercontroller only exist on MY machine AND on the server (check lecture 126), pc from someone else does not exist on my machine so PlayerState !
 */
UCLASS()
class COOP_419_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);
	
	
};
