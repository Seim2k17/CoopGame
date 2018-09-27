// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon_Rifle.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/MeshComponent.h"
#include "SCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Coop_419.h"
#include "Net/UnrealNetwork.h"



static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat
);




ASWeapon_Rifle::ASWeapon_Rifle()
{
	RateOfRire = 600;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	BulletSpread = 2.0f;

	//SetReplicates(true);
}



void ASWeapon_Rifle::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 /RateOfRire;
}

void ASWeapon_Rifle::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}

	//Super::Fire();
	//Trace the world, from pawn eyes to crosshair location
	//UE_LOG(LogTemp, Log, TEXT("Rifle fired. "));
	//who is holding this Weapon
	//AActor* MyOwner = GetOwner();
	AActor* MyOwner = GetAttachParentActor();
	if (MyOwner)
	{
		//Passing by reference
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		
		//Add some randomisation so the weaponFire inside a cone
		//BulletSpread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);


		//EndLocation starts from the Eye in direction of the rotation multiplied with some big number (10k should be far enough to hit all kind of enemy)
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		//set some preferences for the LineTrace
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		//more compley but also more accurate, exact result where we hit something, maybe test, but if we want a headshot f.e. we need this to be true
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		//Particle "Target" Parameter
		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		//Struct which hold all of the Hit Data
		FHitResult Hit;
		//specify where the linetrace will go (from eye to target)
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//Blocking Hit! Process Damage !
			//Draw a debug Line !
			AActor* HitActor = Hit.GetActor();
			
			//WeakObjectPointer
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVUNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
			
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;


		}

		
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 5.f, 0, 1.0f);
		}
		
		PlayFireEffect(TracerEndPoint);

		if (Role == ROLE_Authority)
		{
			//.FromTrace the Client can figure out itself
			HitScanTrace.TraceEnd = TracerEndPoint;

			HitScanTrace.SurfaceType = SurfaceType;

			HitScanTrace.Seed = (HitScanTrace.Seed + 1) % 3;
		}

		LastFiredTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon_Rifle::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon_Rifle::ServerFire_Validate()
{
	return true;
}

void ASWeapon_Rifle::StartFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
		return;
	}

	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon_Rifle::Fire, TimeBetweenShots, true, FirstDelay);
	
}

void ASWeapon_Rifle::ServerStartFire_Implementation()
{
	StartFire();
}

bool ASWeapon_Rifle::ServerStartFire_Validate()
{
	return true;
}

void ASWeapon_Rifle::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon_Rifle::PlayFireEffect(FVector TraceEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		//get the location of the MuzzleEfect
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}
	}

	//APawn* MyOwner = Cast<APawn>(GetOwner());
	APawn* MyOwner = Cast<APawn>(GetAttachParentActor());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//UE_LOG(LogTemp, Log, TEXT("CamShake"));
			PC->ClientPlayCameraShake(FireCameraShakeClass);
		}

	}
}

void ASWeapon_Rifle::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVUNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}



	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation(), true);
	}
}

void ASWeapon_Rifle::OnRep_HitScanTrace()
{
	//Play cosmetic FX
	PlayFireEffect(HitScanTrace.TraceEnd);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceEnd);
}

//Function autogenerated by Engine in HeaderFile
void ASWeapon_Rifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Condition because we do noch send it back to the server and execute it again (?), replicate to everyone but not to the owner who shot the weapon
	DOREPLIFETIME_CONDITION(ASWeapon_Rifle, HitScanTrace, COND_SkipOwner); // include Net/UnrealNetwork.h 

}

