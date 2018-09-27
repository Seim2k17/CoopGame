// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerUpActor.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PowerupInterval = 0.0f;

	TotalNrOfTicks = 0.0f;

	bIsPowerUpActive = false;

	SetReplicates(true);

}


// Called when the game starts or when spawned
void ASPowerUpActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerUpActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExpired();

		bIsPowerUpActive = false;
		OnRep_PowerupActive();

		//Delete Timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerUpActor::ActivatePowerup(AActor* ActiveFor)
{
	//Impl. in BP
	OnActivated(ActiveFor);

	//will be replicated to all the clients
	bIsPowerUpActive = true;
	//OnRep Notifies functions don´t get called on the server, so manually need to call these function on the server
	//if changed on the client this function will be called
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerUpActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}


}

void ASPowerUpActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerUpActive);
}

void ASPowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//not only marked replicated in headerfile but 2ndly do it with the fllw. macro
	DOREPLIFETIME(ASPowerUpActor, bIsPowerUpActive);
}
