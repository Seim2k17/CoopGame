// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ASWeapon::ASWeapon()
{
 	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	//same class will replicate on the client
	SetReplicates(true);

}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ASWeapon::Fire()
{
	UE_LOG(LogTemp, Log, TEXT("BaseClass Fire called"));
	AActor* MyOwner = GetOwner();
	UE_LOG(LogTemp, Log, TEXT("Owner is: %s"),*MyOwner->GetName());
}

void ASWeapon::StartFire()
{

}

void ASWeapon::StopFire()
{

}

