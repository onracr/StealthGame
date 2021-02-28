// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHoleActor.h"
#include "Components/SphereComponent.h"
#include "FPSCharacter.h"

// Sets default values
ABlackHoleActor::ABlackHoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;

	GravitationalField = CreateDefaultSubobject<USphereComponent>(TEXT("GravitationalField"));
	GravitationalField->SetSphereRadius(3000);
	GravitationalField->SetupAttachment(Mesh);

	EventHorizon = CreateDefaultSubobject<USphereComponent>(TEXT("EventHorizon"));
	EventHorizon->SetSphereRadius(100);
	EventHorizon->SetupAttachment(Mesh);

	EventHorizon->OnComponentBeginOverlap.AddDynamic(this, &ABlackHoleActor::OnComponentHit);

}

// Called when the game starts or when spawned
void ABlackHoleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlackHoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent *> OverlappingComponents;
	GravitationalField->GetOverlappingComponents(OverlappingComponents);

	for (auto OverlappingComponent : OverlappingComponents)
	{
		if (OverlappingComponent && OverlappingComponent->IsSimulatingPhysics())
		{
			float SphereRadius = GravitationalField->GetScaledSphereRadius();
			const float ForceStrength = -2000.f;
			// float Force = (OverlappingComponent->GetMass() * Mesh->GetMass()) / FMath::Pow(FVector::Dist(GetActorLocation(), OverlappingComponent->GetComponentLocation()), 2);

			OverlappingComponent->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}
}

void ABlackHoleActor::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Event Horizon is Overlapped!"))
	if (!OtherActor) return;
	OtherActor->Destroy();
}
