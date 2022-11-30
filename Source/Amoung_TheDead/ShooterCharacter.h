// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AmmoType.h"

#include "ShooterCharacter.generated.h"


//Player state
UENUM(BlueprintType)
enum class ECombactState : uint8
{
	ECS_UNOccupied UMETA(DisplayName = "Un occupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "Fire timer in progress"),
	ECS_Reload UMETA(DisplayName = "Realoding"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),

	ECS_MAX UMETA(DisplayName = "Deafult Max")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);


UCLASS()
class AMOUNG_THEDEAD_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: //Variable declarations

//Character - Creaction

	/* Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* cameraBoom;

	/* Camera that follows the Character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* followCamera;

//Movement Section

	/*  Base turn rate in deg per sec (varies from 0 to 1) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	float baseTurnRate;

	/*  Base look up/down rate in deg per sec (varies from 0 to 1) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	float baseLookUpRate;

//Sensitivity 

	// -- sensitivity of movement when we aim in and aim Off --- (varies from 0 to 1)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	float hipLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	float hipTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	float aimLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Section", meta = (AllowPrivateAccess = "true"))
	float aimTurnRate;

//Fire Section

	/* Montage for firing the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* hipFireMontage;

	//BeamParicles

	/* Particles spawned upon fire imact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* impactParticle;

//Continous Fire Section

	/* Fire button pressed */
	bool bFireButtonPressed;

	/* True when fire --- false when waiting for the timer */
	bool bShouldFire;

	/* Timer for gun shots */
	FTimerHandle AutoFireTimer;

//Camera Zoomin--ZoomOut

	/* tells about wheter character is aiming or not --- true when Aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/* Default camera field of view */
	float cameraDefaultFOV;

	/* FOV when character Aim */
	float cameraZoomedFOV;

	/* For interpolation --- smooth zooming --- Current frame FOV */
	float cameraCurrentFOV;

	/* Inter Spped for Zooming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float zoomInterpSpeed;

//Crosshair

	/* Determines the spread of the crosshair */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float crooshairSpreadMultiplier;

	/* Velocity component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float crosshairVelocityFactor;

	/* In air component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float crosshairInAirFactor;

	/* Aim component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float crosshairAimFactor;

	/* Shooting component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float crossShootFactor;

	//crosshair fire respponse

	/*reponse time*/
	float shootTimeDuration;
	bool bBulletFire; // true when bullet fires and false when not shooting
	FTimerHandle crosshairShootTimer;

//Item---Widget section

	/* The Item we are looking at (could be null)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	class AItem* traceHitItem;

	/* True if we need to trace for item (everyFrame) */
	bool bShouldTraceForItems;

	bool bIsWeaponSwapping;

	/* Number of overlappes AItems */
	int8 overlappedItemCount;

	/* AItem we hit last frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitLastFrame;

//Weapon Section

	/* Current Weapon in hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combact", meta = (AllowPrivateAccess = "true"))
	class AWeapon* equippedWeapon;

	/* Set this in BPs for the default weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combact", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> defaultWeaponClass;

//Item interpolation

	//Weapon Interp Section
	/* Distance from camera while interping weapon location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Interpolation", meta = (AllowPrivateAccess = "true"))
	float cameraWeaponInterpDistance;

	/* Elevation from camera while interping weapon location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Interpolation", meta = (AllowPrivateAccess = "true"))
	float cameraWeaponInterpElevation;

	//Ammo interpLocation
	/* Distance from camera while interping weapon location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Interpolation", meta = (AllowPrivateAccess = "true"))
	float cameraAmmoInterpDistance;

	/* Elevation from camera while interping weapon location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Interpolation", meta = (AllowPrivateAccess = "true"))
	float cameraAmmoInterpElevation;

//Bullet section

	/* Stores data about ammo */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int> ammoMap;

	//GameStart Ammo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 starting9mm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 starting556mm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 starting762mm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	int32 startingShells;

//Player State
	/* the state of the player --- unoccupied, reload, firing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ECombactState combactState;

//Reload section

	/* reload montage for reload animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* reloadMontage;

	/* montage to equip gun */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* equipMontage;

	/* transform of the clip when we first grab the clip while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reload", meta = (AllowPrivateAccess = "true"))
	FTransform clipTransform;

	/* to attch to the charcter's hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reload", meta = (AllowPrivateAccess = "true"))
	USceneComponent* handSceneComponent;

//Crouch Section

	/* true when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	//Speed section

	/* regular walk speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float baseMovementSpeed;

	/* crouch walk speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float crouchMovementSpeed;

	/* crouch walk speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float aimMovementSpeed;

	//Capsule HalfHeigh

	/* current capsule halfheight */
	float currentCapsuleHalfHeight;

	/* capsule height while standing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float standingCapsuleHalfHeight;

	/* capsule height while crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float crouchingCapsuleHalfHeight;

	//friction

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float standingFriction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float crouchFriction;

	/* An array of weapons --- (one in hand another is hidden) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> gunsInventory;

	/* Reference to the pistol */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	AWeapon* pistol;

	/*Last weapon before pistol*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	AWeapon* weaponBeforePistol;

	const int32 GUNS_INVENTORY_CAPACITY{2};

	/* Delegate to send info to the gunsWidget when equipping */
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate equipItemDelegate;


public: //Getters and Setters

//Getters

	/* returns cameraBoom */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return cameraBoom; }

	/* returns FollowCamera */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return followCamera; }

	/* return is player shooting---Firing */
	FORCEINLINE bool GetIsPlayerFiring() const { return bBulletFire; }

	/* returns wgeter the player is aiming or not */
	FORCEINLINE bool GetPlayerIsAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpread() const { return crooshairSpreadMultiplier; }

	/* returns how many items are being overlapped with shooter character */
	FORCEINLINE int8 GetOverlappedItemCount() const { return overlappedItemCount; }

	FORCEINLINE ECombactState GetPlayerCombatState() const { return combactState; }

	FORCEINLINE bool GetPlayerCrouching() const { return bCrouching; }
	FORCEINLINE bool GetTraceforItems() const { return bShouldTraceForItems; }

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return equippedWeapon; }

//Setters

	/* Sets the overlapped items */
	void IncrementOverlappedItemCount(int8 amount);
	FORCEINLINE void SetTraceforItems(bool boolean) { bShouldTraceForItems = boolean; }

//Functions

	/* returns the location where weapon needs to be interp */
	FVector GetCameraInterpLocation(AItem* item);

	void GetPickupItem(AItem* item);


protected: //Functions

//Movement

	/* Called for forward/backward input */
	void MoveForward(float value);

	/* Called for right/left input */
	void MoveRight(float value);

	/*right and left turn rate*/
	void TurnAtRate(float value);

	/*up and down rate*/
	void LookAtRate(float value);

	/* jump function make character to jump */
	void jump();

	/* stop charecter jumping */
	void StopJump();

	/*  */
	void CrouchButtonPressed();

//Aim Section

	/* called when aim button is presed --- flip flop */
	void AimON();
	void AimOff();
	void AimingButtonFlipFlop(); // Use pair above this function for Quik scop --- Use this function for Normal Scope

//Fire Section

	/* Called when fire button pressed */
	void FireWeapon();

	/* Fire button pressed and released */
	void FireButtonPressed();
	void FireButtonReleased();

	//FireTimerFunctions
	void StartFireTimer();
	UFUNCTION()
	void AutoFireReset();

	//Things FireWeapon do...
	void PlayFireSound();
	void SendBullet();
	void PlayGunHipFireMontage();

//Reload Section
	
	//Called when reload nutton pressed
	void ReloadButtonPressed();
	//Called from reload press or when bullets are over while firing
	void ReloadWeapon();
	//To stop the reloadmontage while swaping guns
	void StopReloadingWeapon();
	//Will be called from blueprints when reloading is over
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	//Will be called from blueprints when equipping is finished
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

//Camera ZoomIn--ZoomOut

	/* Interpolates the cameraFOV when aim off and On */
	void CameraInterpZoom(float DeltaTime);

//Croshair

	void CalculateCrosshairSpread(float DeltaTime);

	//shoot response
	UFUNCTION()
	void StartBulletFireCrosshairSpread();
	UFUNCTION()
	void FinishBulletFireCrosshairSpread();

//Pickup Widget Section

	/* Line trace items under the crosshair */
	bool TraceUnderCrosshairs(FHitResult& outHitResult);

	/* Trace for items if overlapped item count is greter than 0 */
	void TraceForItems();

//Weapon Section

	/* Spawn the default weapon and equips it */
	AWeapon* SpawnDefaultWeapon();

	/* Takes a weapon and attaches it the to the hand socket */
	void EquipWeapon(AWeapon* weaponToEquip);

	/* Detach the equipped weapon and let it fall */
	void DropWeapon();

	/* Calles when G is pressed */
	void ItemPickupAndDropButtonPressed();
	void ItemPickupAndDropButtonReleased();

	/* Drop cyrrent weapon and equip tracehititem(if its a weapon) */
	void SwapWeapon(AWeapon* weaponToSwap);

//Pistol Section

	/* Throw the pistol away */
	void DropPistol();

	void pickPistol(AWeapon* weapon);

	void EquipPistol();

	void weaponRecovery(AWeapon* weaponToEquip);

	void TakePistol();

//Ammo section

	//set start ammo
	void InitializeAmmoMap();

	bool WeaponHasAmmo();

	/* Checks to see if we have sutable ammp */
	bool CarryingAmmo();

//Reload Clip

	UFUNCTION(BlueprintCallable)
	void GrabClip();

	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

//Walking -- capsule halfheight

	/* Inbterps capsule half height */
	void InterpCapsuleHalfHeight(float DeltaTime);

//Ammo--pickups and handling

	void PickupAmmo(class AAmmo* ammo);

//Weapon Exchange Functions
	void WeaponOneKeyPressed();
	void WeaponTwoKeyPressed();
	void WeaponThreeKeyPressed();

	void ExchangeInventoryItems(int32 currentItemIndex, int32 newItemIndex);

//Weapon mode
	void SetWeaponMode();
};
