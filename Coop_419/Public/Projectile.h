// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"


class UStaticMeshComponent;
class UDamageType;
class UParticleSystem;
class USoundBase;
class USphereComponent;
class UProjectileMovementComponent;


UCLASS()
class COOP_419_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
		UParticleSystem* ExplodeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
		USoundBase* ExplodeSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo")
		USphereComponent* Collisioncomp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		float DamageRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		TSubclassOf<UDamageType> DamageType;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDestroy(float ExplodeDelay, ACharacter* Owner);

	UFUNCTION()
	void Explode(ACharacter* MyOwner);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	
};
