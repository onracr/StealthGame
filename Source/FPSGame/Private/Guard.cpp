// Fill out your copyright notice in the Description page of Project Settings.


#include "Guard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Engine/TargetPoint.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGuard::AGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensing Component"));

	SensingComponent->OnSeePawn.AddDynamic(this, &AGuard::OnPawnSeen);
	SensingComponent->OnHearNoise.AddDynamic(this, &AGuard::OnNoiseHeard);

	GuardState = EGuardState::Idle;
	bFlag = false;
}

// Called when the game starts or when spawned
void AGuard::BeginPlay()
{
	Super::BeginPlay();
	
	StartingRotation = GetActorRotation();
}

// Called every frame
void AGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetPatrolRoute(DeltaTime);
}

void AGuard::SetGuardState(EGuardState NewState)
{
	if (GuardState == NewState) return;

	GuardState = NewState;
	OnRep_GuardState();	// for server!

}

void AGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGuard, GuardState); // "Net/UnrealNetwork.h"
}

void AGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn) return;
	UE_LOG(LogTemp, Warning, TEXT("Guard saw %s"), *SeenPawn->GetName())

	AFPSGameMode *GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}
	SetGuardState(EGuardState::Alerted);
}

void AGuard::OnNoiseHeard(APawn* NoiseSource, const FVector& Location, float Volume)
{
	if (GuardState == EGuardState::Alerted) return;
	if (!NoiseSource) return;

	UE_LOG(LogTemp, Warning, TEXT("Guard heard %s"), *NoiseSource->GetName())

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = Direction.Rotation();
	NewLookAt.Pitch = .0f;
	NewLookAt.Roll = .0f;

	SetActorRotation(NewLookAt);
	
	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AGuard::ResetOrientation, 3.f);

	SetGuardState(EGuardState::Suspicious);
}

void AGuard::ResetOrientation()
{
	SetActorRotation(StartingRotation);
	SetGuardState(EGuardState::Idle);
}

/**
 * Simple AI Behaviour
 */

void AGuard::SetPatrolRoute(float DeltaTime)
{
	if (GuardState == EGuardState::Suspicious) return;

	if (PatrolPoints.Num() > 0)
	{
		if (FVector::Dist(GetActorLocation(), PatrolPoints[bFlag]->GetActorLocation()) < 100.f)
			bFlag = !bFlag;
		FVector NewLocation(FMath::VInterpTo(GetActorLocation(), PatrolPoints[bFlag]->GetActorLocation(), DeltaTime, .2f));
		SetActorRelativeLocation(NewLocation);

		// UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), PatrolPoints[bFlag]);
		
	}
}