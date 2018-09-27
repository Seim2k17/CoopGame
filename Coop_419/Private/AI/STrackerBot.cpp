// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this,&ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	
	bUsedVelocityChange = true;
	MovementForce = 1000;

	RequiredDistanceToTarget = 100.0f;

	ExplosionRadius = 350.0f;
	ExplosionDamage = 60.0f;

	SelfDamageInterval = 0.4f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	AActor* BestTarget = null;
	//very big Value assigned
	float NearestTargetDistance = FLT_MAX;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
		{
			//we´re only looking for enemies
			continue;
		}

		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
		{
			float DistanceTestPawn = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
			if (DistanceTestPawn < NearestTargetDistance)
			{
				BestTarget = TestPawn;
				NearestTargetDistance = DistanceTestPawn;
			}
		}
	}

	if (BestTarget)
	{
		UNavigationPath* TrackerBotPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, ASTrackerBot::GetNextPathPoint, 5.0f, false);

		//need to have more than 1 elements (1= Botposition)
		if (TrackerBotPath && TrackerBotPath->PathPoints.Num() > 1)
		{
			//return next Pathpoint in the path
			return TrackerBotPath->PathPoints[1];
		}

	}


	

	//failed to find path
	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{


	//@TODO: pulse the material on hit
	if (MatInst == nullptr)
	{
		//need to use dynamic Material when changing mat on runtime, otherwise every instance in the Game will change the Material to!
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		//take Name which we use from the MaterialEditor
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Health: %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	//Explode on Hitpoints = 0
	if (Health <= 0.0f)
	{
		SelfDestruct();
	}

}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}
		bExploded = true;

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, GetActorLocation());

		MeshComp->SetVisibility(false, true);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		if (Role == ROLE_Authority)
		{

			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(this);

			UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 16, FColor::Red, false, 2.0f, 0, 1.5f);

			//delete Actor imm. -> On client it will give not enoug time to spawn the explosion fx
			//Destroy();
			//so set LifeTime and hide mesh
			
			SetLifeSpan(2.0f);
		}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded)
	{

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();

			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!", NULL, FColor::Red, 3.0f);

		}
		else
		{
			//Keep Moving to Next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUsedVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 1.0f);
		}


		DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 12, FColor::Red, false, 0.0f, 0, 1.0f);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//make sure not to losse BP functionality
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction && !bExploded)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn && !USHealthComponent::IsFriendly(OtherActor,this))
		{
			//We overlappe with a player

			if (Role == ROLE_Authority)
			{
				//Start Self Destruction sequence
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			}
			
			bStartedSelfDestruction = true;

			//because the TrackerBot moves we need SpawnSoundAttached !, no need to check for null it´s already done in the Method
			UGameplayStatics::SpawnSoundAttached(SelfDestructSFX,RootComponent);
		}
	}

	
}

