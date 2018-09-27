// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon_GLauncher.h"
#include "GameFramework/Character.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"


ASWeapon_GLauncher::ASWeapon_GLauncher()
{
	//Super::ASWeapon();

	ExplodeDelay = 1.5f;
	MuzzleSocketName = "MuzzleSocket";

	RateOfRire = 3;

	//same class will replictae on the client
	//SetReplicates(true);
}


void ASWeapon_GLauncher::Fire()
{
	//Super::Fire();
	UE_LOG(LogTemp, Log, TEXT("GLauncher fired"));
	ACharacter* MyOwner = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);

	if (MyOwner && Grenade)
	{

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = Cast<APawn>(GetParentActor());
				
		//Passing by reference
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		//spawn projectile at muzzle
		GrenadeRef = GetWorld()->SpawnActor<AProjectile>(Grenade, MuzzleLocation, EyeRotation, ActorSpawnParams);
		
		GrenadeRef->SetDestroy(ExplodeDelay, MyOwner);

	}
	
}

void ASWeapon_GLauncher::StartFire()
{

}

void ASWeapon_GLauncher::StopFire()
{

}
