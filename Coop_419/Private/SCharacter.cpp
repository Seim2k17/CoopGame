// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "SHealthComponent.h"
#include "Coop_419.h"
#include "SWeapon.h"
#include "Net/UnrealNetwork.h"
#include "SENUM_Weapons.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp->SetupAttachment(SpringArmComp);

	//need to enable crouching !
	//teh capsule component is compressed when crouch is activated
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	WeaponSocketName = "WeaponRight_Socket";

	ZoomedFOV = 65.f;
	ZoomedInterpSpeed = 20;



}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (StarterWeapon != nullptr)
	{
		if (StarterWeapon == RifleClassBP)
		{
			WeaponToChange(EWeaponTypes::Rifle, RifleClassBP);
		}

		if (StarterWeapon == GLauncherClassBP)
		{
			WeaponToChange(EWeaponTypes::Launcher, GLauncherClassBP);
		}
		
		if (StarterWeapon == AIRifleClassBP)
		{
			WeaponToChange(EWeaponTypes::AIRifle, AIRifleClassBP);
		}
	}
}


void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}


void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}


void ASCharacter::BeginCrouch()
{
	Crouch();
}


void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginJump()
{
	Jump();
}

void ASCharacter::EndJump()
{
	StopJumping();
}

void ASCharacter::WeaponChange()
{
	if (Role < ROLE_Authority)
	{
		ServerWeaponChange();
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("Weapon Changed to ..."));
	
	switch (WeaponTypes)
	{
		//Weapon Cycle
		case EWeaponTypes::none : 
			WeaponToChange(EWeaponTypes::Rifle, RifleClassBP);
			//UE_LOG(LogTemp, Log, TEXT(".. Rifle."));
			break;
		case EWeaponTypes::Rifle :
			WeaponToChange(EWeaponTypes::Launcher, GLauncherClassBP);
			//UE_LOG(LogTemp, Log, TEXT(".. GLauncher."));
			break;
		case EWeaponTypes::Launcher:
			WeaponToChange(EWeaponTypes::AIRifle, AIRifleClassBP);
			//UE_LOG(LogTemp, Log, TEXT(".. GLauncher."));
			break;
		case EWeaponTypes::AIRifle:
			WeaponToChange(EWeaponTypes::none, nullptr);
			//UE_LOG(LogTemp, Log, TEXT(".. none."));
			break;
	}

	
}

void ASCharacter::ServerWeaponChange_Implementation()
{
	WeaponChange();
}

bool ASCharacter::ServerWeaponChange_Validate()
{
	return true;
}

void ASCharacter::WeaponToChange(EWeaponTypes WeaponChanged, TSubclassOf<ASWeapon> WeaponClass)
{

	if(Weaponref)
	{
		Weaponref->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Weaponref->Destroy();
		Weaponref = nullptr;
	}

	WeaponTypes = WeaponChanged;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//FString WeaponString = EnumAsString(WeaponToChange, EWeaponTypes::Type);
	//UE_LOG(LogTemp, Log,TEXT(": "),*FString(WeaponString));

	if (WeaponClass)
	{
		Weaponref = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, GetTransform(), SpawnParams);
		WeaponAttach();
	}

}

void ASCharacter::WeaponAttach()
{
	Weaponref->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
}

void ASCharacter::StartFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
		return;
	}

	if (Weaponref)
	{
		//UE_LOG(LogTemp, Log, TEXT("Weapon is fired: "));
		Weaponref->StartFire();
	}
}

void ASCharacter::ServerStartFire_Implementation()
{
	StartFire();
}

bool ASCharacter::ServerStartFire_Validate()
{
	return true;
}

void ASCharacter::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
		return;
	}

	if (Weaponref)
	{
		Weaponref->StopFire();
	}
}

void ASCharacter::ServerStopFire_Implementation()
{
	StopFire();
}

bool ASCharacter::ServerStopFire_Validate()
{
	return true;
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{

		bDied = true;

		//Die!
		UE_LOG(LogTemp, Warning, TEXT("CHARACTER DIED!"));
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(6.0f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomedInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}
 
// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::BeginJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::EndJump);

	PlayerInputComponent->BindAction("WeaponChange", IE_Pressed, this, &ASCharacter::WeaponChange);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)

	{
		return CameraComp->GetComponentLocation();
	}
	//Fallback
	return Super::GetPawnViewLocation();
}


//2.nd Step of Replicating Variables: define WHAT we want and HOW e want to replicate the Variables
void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//when we spawn it on the server we replicate this on the client
	DOREPLIFETIME(ASCharacter, Weaponref); // include Net/UnrealNetwork.h 
	DOREPLIFETIME(ASCharacter, WeaponTypes); // include Net/UnrealNetwork.h 
	DOREPLIFETIME(ASCharacter, bDied); 
}
