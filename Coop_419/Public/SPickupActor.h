// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerUpActor;

UCLASS()
class COOP_419_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	void Respawn();

	ASPowerUpActor* PowerUpInstance;

	FTimerHandle TimerHandle_RespawnTimer;

public:	

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		TSubclassOf<ASPowerUpActor> PowerUpClass;
	
	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		float CoolDownDuration;
	
};
