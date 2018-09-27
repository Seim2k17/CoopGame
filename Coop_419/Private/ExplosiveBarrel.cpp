// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplosiveBarrel.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"

USTRUCT()
struct ActorHit
{
	UPROPERTY()
	FHitResult HitRes;
	UPROPERTY()
	AActor* HitActor;
};

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{

	PrimaryActorTick.bCanEverTick = true;
 	
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	//RootComponent = RadialForceComp;
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;

	bUseRadialForceComp = false;

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	RootComponent = BarrelMesh;
	//BarrelMesh->SetupAttachment(RadialForceComp);
	RadialForceComp->SetupAttachment(BarrelMesh);
	
	BarrelMesh->SetSimulatePhysics(true);
	BarrelMesh->SetMobility(EComponentMobility::Movable);
	BarrelMesh->SetIsReplicated(true);

	BarrelTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BarrelTrigger"));
	BarrelTrigger->SetupAttachment(BarrelTrigger);

	BarrelTrigger->OnComponentBeginOverlap.AddDynamic(this, &AExplosiveBarrel::HandleBarrelBeginOverlap);


	MaterialIntact = CreateDefaultSubobject<UMaterial>(TEXT("IntactMaterial"));
	MaterialExploded = CreateDefaultSubobject<UMaterial>(TEXT("ExplodedMaterial"));

	SetReplicates(true);
	SetReplicateMovement(true);

}

void AExplosiveBarrel::Tick(float DeltaTime)
{
	if (bIsBurning)
	{
		if (BurnFX)
		{
			
		}
	}
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (BarrelMesh) BarrelMesh->SetMaterial(0, MaterialIntact);
	
}

void AExplosiveBarrel::OnRep_Exploded()
{
	//only audiovisual effects, no gameplay relevance
	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorTransform(), true);
	}

	if (ExplosionSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSFX, GetActorLocation());
	}

	if (MaterialExploded) BarrelMesh->SetMaterial(0, MaterialExploded);

	if (ParticleSystemReference) ParticleSystemReference->DestroyComponent();
}


void AExplosiveBarrel::HandleBarrelBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
}

void AExplosiveBarrel::Explode()
{

	OnRep_Exploded();

	if (bUseRadialForceComp)
	{
		RadialForceComp->FireImpulse();
		BarrelMesh->AddImpulseAtLocation(ImpulseVector * 500, GetActorLocation());
	}

	if (bIsDead == false)
	{
	

		bIsDead = true;

		//Damage
		TArray<AActor*> IgnoredActors;
		UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageTypeClass, IgnoredActors,this,GetInstigatorController(),true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 2.0f, 0, 2.0f);

		if (!bUseRadialForceComp)
		{
			//check for surrounded boxes

			TArray<FHitResult> HitList;
			FCollisionShape collShape;
			FCollisionQueryParams queryParams;
			collShape.ShapeType = ECollisionShape::Sphere;
			collShape.SetSphere(DamageRadius);

			queryParams.bTraceComplex = true;
			FVector startLoc = GetActorLocation();
			TArray<ActorHit> ActorList;

			GetWorld()->SweepMultiByChannel(HitList, startLoc, startLoc + DamageRadius, FQuat(1.0f, 1.0f, 1.0f, 1.0f), ECollisionChannel::ECC_PhysicsBody, collShape, queryParams);

			if (HitList.Num() > 0)
			{
				for (FHitResult hit : HitList)
				{
					ActorHit HittedActor;
					HittedActor.HitActor = hit.GetActor();
					HittedActor.HitRes = hit;
					//if (ActorList.Find(HittedActor) == false) 
					if (!(hit.GetActor() == this)) ActorList.Add(HittedActor);
					UE_LOG(LogTemp, Log, TEXT("Name: %s"), *hit.GetActor()->GetName());
				}
				//return ActorList;
			}
			UE_LOG(LogTemp, Log, TEXT("ActorList: %d"), ActorList.Num());



			if (BarrelMesh)
			{
				//Physics for itself
				BarrelMesh->AddImpulseAtLocation(ImpulseVector * 500, GetActorLocation());
				//and other with some kind of damping

				TArray<AActor*> PhysicalBoxes;


				// Just Radius and HitActors of Class 
				for (ActorHit var : ActorList)
				{
					TArray<UStaticMeshComponent*> StaticComps;
					var.HitActor->GetComponents<UStaticMeshComponent>(StaticComps);
					for (UStaticMeshComponent* MeshComp : StaticComps)
					{
						UE_LOG(LogTemp, Log, TEXT("IMPULSE"));
						MeshComp->AddImpulseAtLocation(ImpulseBoxes * 300, var.HitRes.Location);
					}

				}
			}
		}
	}
	
}


void AExplosiveBarrel::Burn()
{
	if (!bIsBurning)
	{
		OnRep_Burning();
		bIsBurning = true;
		GetWorldTimerManager().SetTimer(TimerHandle_ExplodeTimer, this, &AExplosiveBarrel::Explode, 1.0f, false, 3.0f);
	}
}

void AExplosiveBarrel::OnRep_Burning()
{
	ParticleSystemReference = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BurnFX, GetActorLocation());
}


//2.nd Step of Replicating Variables: define WHAT we want and HOW e want to replicate the Variables
void AExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//when we spawn it on the server we replicate this on the client
	DOREPLIFETIME(AExplosiveBarrel, bIsDead); // include Net/UnrealNetwork.h 
	DOREPLIFETIME(AExplosiveBarrel, bIsBurning); // include Net/UnrealNetwork.h 
}


