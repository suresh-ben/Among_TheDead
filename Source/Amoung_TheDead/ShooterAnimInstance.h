// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"

#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AMOUNG_THEDEAD_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();

	/* It works like begin play */
	virtual void NativeInitializeAnimation() override;

	/* It works like Tick function */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:

	/* points to player character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* shooterCharacter;

//Movement parameters

	//Idle-Shift variable
	//Time to switch btween Idle Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float IdleTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIdleForTooLong;
	bool StartIdleTimer;
	FTimerHandle IdleTimerHandle;

	/* The Speed of the Character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float speed;

	/* Wheter or not the player is in air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	/* Whether or not the player is moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/* It's the difference between direction of player and his velocity direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float movementOffsetYaw;

	/* Offset yaw --- the frame before we are stop moving */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float lastMovementOffsetYaw;

	/* True when player do aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combact", meta = (AllowPrivateAccess = "true"))
	bool bIsAimimg;

//TIPO -- turn in place
	/* Yez (z) of the character this frame */
	float TIPcharcterYaw;

	/* Yez (z) of the character the previous frame */
	float TIPcharcterYawLastFrame;

	/* to attch to the charcter's hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
	float rootYawOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
	float pitch;

	/* rotation curve value this frame */
	float rotationCurveValue;

	/* rotation curve value the last frame */
	float rotationCurveValueLastFrame;

	/* true while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combact", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

// LEan

	/* charcter rotation this frame */
	FRotator characterRotation;

	/* character yaw last frame */
	FRotator charcterRotationLastFrame;

	/* used for leaning in blendspace */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float yawDelta;

//Crouch

	/* True when crouchong */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	/* blend weight for recoil */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float recoilWeight;

//Weapons

	/* weapon type of the currently equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	EWeaponType equippedWeaponType;

	/* False while reloading, equipping */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;

protected:

	UFUNCTION(BlueprintCallable)
	void StartIdleAnimTimer();
	UFUNCTION()
	void FinishIdleAnimTimer();

	/* Handles turn in place variables */
	void TurnInPlace();

	/*Handles leaning */
	void Lean(float DeltaTime);
};
