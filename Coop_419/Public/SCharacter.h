// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SENUM_Weapons.h"
#include "SCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;
class ASWeapon;

UCLASS()
class COOP_419_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	/** to execute this in the BT*/
	UFUNCTION(BlueprintCallable, Category = "Weaon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weaon")
	void StartFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASWeapon> StarterWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASWeapon> RifleClassBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASWeapon> GLauncherClassBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<ASWeapon> AIRifleClassBP;

	//1rst Step of Replication Variables
	UPROPERTY(Replicated)
	ASWeapon* Weaponref;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponSocketName;

	void BeginCrouch();

	void EndCrouch();

	void BeginJump();

	void EndJump();

	void WeaponChange();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerWeaponChange();

	void WeaponToChange(EWeaponTypes WeaponChanged, TSubclassOf<ASWeapon> WeaponClass);

//needable ?
//  	UFUNCTION(Server, Reliable, WithValidation)
//  	void ServerWeaponToChange(EWeaponTypes WeaponChanged, TSubclassOf<ASWeapon> WeaponClass);

	void WeaponAttach();


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	void BeginZoom();

	void EndZoom();

	bool bWantsToZoom;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	/** Pawn died previously*/
	bool bDied;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
		class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1f, ClampMax = 100.f))
	float ZoomedInterpSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	/** Default Field of View set during BeginPlay */
	float DefaultFOV;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//it still exist so we can override it , but with our own implementation
	virtual FVector GetPawnViewLocation() const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponTypes WeaponTypes;
	
};
