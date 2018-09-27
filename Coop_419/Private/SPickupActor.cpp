// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "SPowerUpActor.h"


// Sets default values
ASPickupActor::ASPickupActor()
{
 
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;
	

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	//rorate the decal taht it projects correct to the ground (BP_Preview)
	DecalComp->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(SphereComp);

	CoolDownDuration = 10.0f;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		Respawn();
	}

	
}

void ASPickupActor::Respawn()
{

	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("PowerUpClass is not set in %s. Please update your BP."), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerUpInstance = GetWorld()->SpawnActor<ASPowerUpActor>(PowerUpClass, GetTransform(), SpawnParam);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//make sure not to losse BP functionality
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CoolDownDuration);
	}

}

