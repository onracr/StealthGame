// Fill out your copyright notice in the Description page of Project Settings.


#include "Guard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"

// Sets default values
AGuard::AGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Sensing Component"));

	SensingComponent->OnSeePawn.AddDynamic(this, &AGuard::OnPawnSeen);
	SensingComponent->OnHearNoise.AddDynamic(this, &AGuard::OnNoiseHeard);

	GuardState = EGuardState::Idle;
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

}

void AGuard::SetGuardState(EGuardState NewState)
{
	if (GuardState == NewState) return;

	GuardState = NewState;

	OnStateChanged(NewState);
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