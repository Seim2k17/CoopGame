// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SWeapon_GLauncher.generated.h"


class AProjectile;
/**
 * 
 */
UCLASS()
class COOP_419_API ASWeapon_GLauncher : public ASWeapon
{
	GENERATED_BODY()

	public:
		ASWeapon_GLauncher();

	protected:

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
		TSubclassOf<AProjectile> Grenade;

		UPROPERTY()
		AProjectile* GrenadeRef;

		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
		float ExplodeDelay;

		void Fire() override;
		//void Fire();

		void StartFire() override;

		void StopFire() override;
};
