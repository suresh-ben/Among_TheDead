// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "ShooterCharacter.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	bIsAimimg(false),

	IdleTime(10.f),
	bIdleForTooLong(false),
	StartIdleTimer(true),

	TIPcharcterYaw(0.f),
	TIPcharcterYawLastFrame(0.f),
	rootYawOffset(0.f),

	rotationCurveValue(0.f),
	rotationCurveValueLastFrame(0.f),

	characterRotation(0.f),
	charcterRotationLastFrame(0.f),

	recoilWeight(1.f),
	equippedWeaponType(EWeaponType::EWT_MAX),
	bShouldUseFABRIK(true)

{

}

//It is like BeginPlay
void UShooterAnimInstance::NativeInitializeAnimation()
{
	shooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!shooterCharacter)
	{
		shooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (shooterCharacter)
	{
		//Get the lateral speed of the player from velocity
		FVector velocity{ shooterCharacter->GetVelocity() };
		velocity.Z = 0;
		speed = velocity.Size();

		//Is player in the air
		bIsInAir = shooterCharacter->GetCharacterMovement()->IsFalling();

		//Is player Accelerating
		if (shooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//Setting Movement Offset
		FRotator AimRotation{ shooterCharacter->GetBaseAimRotation() }; //Aim roation
		FRotator MovementRoatation{ UKismetMathLibrary::MakeRotFromX(shooterCharacter->GetVelocity()) }; //Velocity Roatation

		movementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRoatation, AimRotation).Yaw;

		if (shooterCharacter->GetVelocity().Size())
		{
			lastMovementOffsetYaw = movementOffsetYaw;
		}

		bIsAimimg = shooterCharacter->GetPlayerIsAiming();

		//Setting Idle switch
		if (!bIsAccelerating && StartIdleTimer)
		{
			StartIdleAnimTimer();
		}
		if (bIsAccelerating || shooterCharacter->GetIsPlayerFiring() || shooterCharacter->GetPlayerIsAiming())
		{
			bIdleForTooLong = false;
			StartIdleTimer = true;
		}
		//reloading or not
		bReloading = shooterCharacter->GetPlayerCombatState() == ECombactState::ECS_Reload;

		bCrouching = shooterCharacter->GetPlayerCrouching();
		bCrouching ? recoilWeight = 0.1f : recoilWeight = 1.f;

		if (shooterCharacter->GetEquippedWeapon())
		{
			equippedWeaponType = shooterCharacter->GetEquippedWeapon()->GetWeaponType();
		}
		
		//When to use FABRIK
		bShouldUseFABRIK = (shooterCharacter->GetPlayerCombatState() == ECombactState::ECS_UNOccupied || shooterCharacter->GetPlayerCombatState() == ECombactState::ECS_FireTimerInProgress);
	}
	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterAnimInstance::StartIdleAnimTimer()
{
	bIdleForTooLong = false;
	StartIdleTimer = false;
	shooterCharacter->GetWorldTimerManager().SetTimer(IdleTimerHandle, this, &UShooterAnimInstance::FinishIdleAnimTimer, IdleTime);
}

void UShooterAnimInstance::FinishIdleAnimTimer()
{
	bIdleForTooLong = true;
}

void UShooterAnimInstance::TurnInPlace()
{
	if (!shooterCharacter) return;

	pitch = shooterCharacter->GetBaseAimRotation().Pitch;


	if (speed || shooterCharacter->GetIsPlayerFiring() || shooterCharacter->GetPlayerIsAiming())
	{
		//Dont need to do turn in place
		rootYawOffset = 0.f;
		TIPcharcterYaw = shooterCharacter->GetActorRotation().Yaw;
		TIPcharcterYawLastFrame = TIPcharcterYaw;
		rotationCurveValueLastFrame = 0.f;
		rotationCurveValue = 0.f;
	}
	else
	{
		TIPcharcterYawLastFrame = TIPcharcterYaw;
		TIPcharcterYaw = shooterCharacter->GetActorRotation().Yaw;
		const float TIPyawDelta{ TIPcharcterYaw - TIPcharcterYawLastFrame };

		//Clamped [-180, 180]
		rootYawOffset = UKismetMathLibrary::NormalizeAxis(rootYawOffset - TIPyawDelta);

		//1.0 for turning and 0 when not turning
		//turning
		const float turning{ GetCurveValue(TEXT("Turning")) };
		if (turning)
		{
			rotationCurveValueLastFrame = rotationCurveValue;
			rotationCurveValue = GetCurveValue(TEXT("Rotation"));

			const float deltaRotation{ rotationCurveValue - rotationCurveValueLastFrame };

			rootYawOffset -= deltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(rootYawOffset) };
			if (ABSRootYawOffset > 90)
			{
				const float yawExcess{ ABSRootYawOffset - 90.f };
				rootYawOffset > 0 ? rootYawOffset -= yawExcess : rootYawOffset += yawExcess;
			}
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (shooterCharacter == nullptr) return;

	charcterRotationLastFrame = characterRotation;
	characterRotation = shooterCharacter->GetActorRotation();

	const FRotator delta{ UKismetMathLibrary::NormalizedDeltaRotator(characterRotation, charcterRotationLastFrame)};

	const float target{ delta.Yaw / DeltaTime };
	const float Interp{ FMath::FInterpTo(yawDelta, target, DeltaTime, 6.f) };
	yawDelta = FMath::Clamp(Interp, -90.f, 90.f);

}
