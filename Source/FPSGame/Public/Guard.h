// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EGuardState : uint8
{
	Idle,
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGuard();

protected:
	UPROPERTY(VisibleAnywhere, Category=Components)
	UPawnSensingComponent* SensingComponent;

	FRotator StartingRotation;
	FTimerHandle TimerHandle;
	EGuardState GuardState;

	virtual void BeginPlay() override;

	void SetGuardState(EGuardState NewState);

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);
	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseSource, const FVector& Location, float Volume);
	UFUNCTION()
	void ResetOrientation();

	UFUNCTION(BlueprintImplementableEvent, Category = AI)
	void OnStateChanged(EGuardState NewState);

public:
	virtual void Tick(float DeltaTime) override;
};