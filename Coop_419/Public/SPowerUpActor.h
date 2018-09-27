// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

UCLASS()
class COOP_419_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**Time between PowerUp Ticks*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PowerUps")
	float PowerupInterval;

	/** Total times we apply the PowerUpEffect*/
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int32 TotalNrOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	//Total Nr of Ticks apllied
	int32 TicksProcessed;

	UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
	bool bIsPowerUpActive;

	UFUNCTION()
		void OnRep_PowerupActive();

	UFUNCTION()
	void OnTickPowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnPowerupStateChanged(bool bNewIsActive);



public:	

	void ActivatePowerup(AActor* ActivateFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnExpired();


};
