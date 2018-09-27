// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SWeapon_Rifle.generated.h"


//Contains Information of a single LineTraceWeapon
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	//to properly replicate we need to pass TEnumAsByte
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceEnd;

	UPROPERTY()
		uint8 Seed = 0;

};

/**
 * 
 */
UCLASS()
class COOP_419_API ASWeapon_Rifle : public ASWeapon
{
	GENERATED_BODY()
public:
	ASWeapon_Rifle();
	
	/** BulletSpread in Degrees*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin=0.0f))
	float BulletSpread;

protected:

	virtual void BeginPlay() override;

	void Fire() override;

	//for high frequency use reliable
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	void PlayImpactEffects(EPhysicalSurface Surface, FVector ImpactPoint);
	
	void PlayFireEffect(FVector TraceEndPoint);

	void StartFire() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();

	void StopFire() override;

	//RepUsing = we trigger a function everytime we Replicate this
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
	
};
