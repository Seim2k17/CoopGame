// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Kismet//GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Use a sphere for simple collision check
	Collisioncomp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	Collisioncomp->InitSphereRadius(22.f);
	Collisioncomp->SetCollisionProfileName("Projectile");

	Collisioncomp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	//Players cant walk on it
	Collisioncomp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	Collisioncomp->CanCharacterStepUpOn = ECB_No;
	RootComponent = Collisioncomp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(Collisioncomp);

	//Use a ProjectileMovementComponent to govern the movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collisioncomp;
	ProjectileMovement->InitialSpeed = 2500.f;
	ProjectileMovement->MaxSpeed = 2500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	
	InitialLifeSpan = 3.0f;



	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::SetDestroy(float ExplodeDelay, ACharacter* Owner)
{
	UE_LOG(LogTemp, Log, TEXT("Time to explode: %s"), *(FString::SanitizeFloat(ExplodeDelay)));

	//Execute Method with Parameter we need a Delegate
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle_DestroyProjectile;
	TimerDel.BindUFunction(this, FName("Explode"), Owner);

	GetWorldTimerManager().SetTimer(TimerHandle_DestroyProjectile,TimerDel, ExplodeDelay, false);
}

void AProjectile::Explode(ACharacter* Owner)
{
	if (ExplodeEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeEffect, GetActorLocation(),GetActorRotation(),true);
	}
	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodeSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;
	//UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
	
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageType, IgnoredActor, this, Owner->GetInstigatorController(), false, ECollisionChannel::ECC_Visibility);
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 1.f, 0, 2.f);

	this->Destroy();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		OtherComp->SetEnableGravity(true);
	}

}

