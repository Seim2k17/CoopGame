// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;
class UCapsuleComponent;
class UParticleSystemComponent;
class URadialForceComponent;


UCLASS()
class COOP_419_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Exploded();

	UFUNCTION()
	void OnRep_Burning();

	UFUNCTION()
	void HandleBarrelBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Explosive")
	void Explode();


	UFUNCTION(BlueprintCallable, Category = "Explosive")
	void Burn();

public:	

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* BarrelMesh;

	UPROPERTY(EditAnywhere)
		UMaterial* MaterialIntact;

	UPROPERTY(EditAnywhere)
		UMaterial* MaterialExploded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UParticleSystem* ExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UParticleSystem* BurnFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		USoundBase* ExplosionSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCapsuleComponent* BarrelTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float DamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<AActor> PhysicBoxClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		FVector	ImpulseVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		FVector ImpulseBoxes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		URadialForceComponent* RadialForceComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		bool bUseRadialForceComp;
	
protected:

	FTimerHandle TimerHandle_ExplodeTimer;

	UPROPERTY(ReplicatedUsing = OnRep_Exploded)
	bool bIsDead;

	UPROPERTY(ReplicatedUsing = OnRep_Burning)
	bool bIsBurning;

 	UPROPERTY()
 	UParticleSystemComponent* ParticleSystemReference;
};
