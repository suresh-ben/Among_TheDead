// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Weapon.h"
#include "Ammo.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AShooterCharacter::AShooterCharacter() :
	//Base Turn Rates
	baseTurnRate(1.f),
	baseLookUpRate(1.f),
	bAiming(false),
	//TurnRate for Aimimg and not Aiming
	hipTurnRate(1.f),
	hipLookUpRate(1.f),
	aimTurnRate(0.5f),
	aimLookUpRate(0.5f),
	//Fire Section
	bShouldFire(true),
	bFireButtonPressed(false),
	//Camera FOV 
	cameraDefaultFOV(0), //We are setting this in Beginplay
	cameraZoomedFOV(45.f),
	cameraCurrentFOV(0.f),
	zoomInterpSpeed(20.f),
	//Crosshair
	crooshairSpreadMultiplier(0.f),
	crosshairVelocityFactor(0.f),
	crosshairInAirFactor(0.f),
	crosshairAimFactor(0.f),
	crossShootFactor(0.f),
	//crosshair response - crosshair shooting movement
	shootTimeDuration(0.05f),
	bBulletFire(false),
	//Item---Widget
	bShouldTraceForItems(false),
	//Interp
	//Weapon
	cameraWeaponInterpDistance(300.f),
	cameraWeaponInterpElevation(65.f),
	//Ammo
	cameraAmmoInterpDistance(400.f),
	cameraAmmoInterpElevation(30.f),
	//Ammo section
	starting9mm(40.f),
	starting762mm(40.f),
	startingShells(40.f),
	starting556mm(40.f),
	//PlayerState
	combactState(ECombactState::ECS_UNOccupied),
	//crouch --- movememt
	bCrouching(false),
		//Speed
	baseMovementSpeed(650.f),
	crouchMovementSpeed(400.f),
	aimMovementSpeed(300.f),
		//capsule
	standingCapsuleHalfHeight(88.f),
	crouchingCapsuleHalfHeight(44.f),
		//friction
	standingFriction(2.f),
	crouchFriction(100.f),
	bIsWeaponSwapping(false),
	pistol(nullptr),
	weaponBeforePistol(nullptr)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Camera Section

	//CameraBoom Setup (Pulls in towards charcter if there is any collision)s
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom")); //creating Camera boom
	cameraBoom->SetupAttachment(RootComponent); //Attaching camera boom to Rootcoponent
	cameraBoom->TargetArmLength = 150.f; // Camera follows Character at this distance
	cameraBoom->bUsePawnControlRotation = true; //Rotate cameraBoom based on the Character
	cameraBoom->SocketOffset = FVector(0.f, 40.f, 75.f);

	//Follow camera Setup
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FOllow Camera")); //Cerating follow camera
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); //Attaching camera to spring arm
	followCamera->bUsePawnControlRotation = false; // camera wont rorate..it follows character

	//Rotation of the character when the controller roatates
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true; // we want rotate character in z-axis

	//Configure Character Movement
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//clip Section
	handSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (followCamera)
	{
		cameraDefaultFOV = GetFollowCamera()->FieldOfView;
		cameraCurrentFOV = cameraDefaultFOV;
	}

	//Spawn the default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());
	gunsInventory.Add(equippedWeapon);
	equippedWeapon->SetslotIndex(0);

	//Set AmmoMap
	InitializeAmmoMap();
	//Set deafult speed
	GetCharacterMovement()->MaxWalkSpeed = baseMovementSpeed;
	
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handles interpolation for Zooming in and Zooming off
	CameraInterpZoom(DeltaTime);
	
	//Calculate crosshair spread multiplier
	CalculateCrosshairSpread(DeltaTime);

	//Check itemcount then Trace and show Item widget
	TraceForItems();

	//Set Capsule Half Height --- interpolation based on bcrouncing
	InterpCapsuleHalfHeight(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent)

	//Binding Axiscfunctions

//Movements

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward); //Binding Forwad Axis
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight); //Binding Right Axis
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AShooterCharacter::TurnAtRate); //Binding Forwad Axis
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShooterCharacter::LookAtRate); //Binding Right Axis

	//Binding Actions

//Movements

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AShooterCharacter::jump); //Binding jump action
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AShooterCharacter::StopJump); //Binding jump action

//Fire section

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AShooterCharacter::FireButtonPressed); //Binding jump action
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AShooterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction(TEXT("AimingButton"), IE_Pressed, this, &AShooterCharacter::AimON); //Turn the scope ON
	PlayerInputComponent->BindAction(TEXT("AimingButton"), IE_Released, this, &AShooterCharacter::AimOff); //Turen the scope Off
  //PlayerInputComponent->BindAction(TEXT("AimingButton"), IE_Pressed, this, &AShooterCharacter::AimingButtonFlipFlop); //Flip the Aimimg when aimimg button pressed
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed); //Bindng Reload Function

//Item pickupsection
	PlayerInputComponent->BindAction(TEXT("DropAndEquipWeapon"), IE_Pressed, this, &AShooterCharacter::ItemPickupAndDropButtonPressed); //Binding item pickup action
	PlayerInputComponent->BindAction(TEXT("DropAndEquipWeapon"), IE_Released, this, &AShooterCharacter::ItemPickupAndDropButtonReleased);
//Crouch
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed); //Binding item pickup action

//Weapons
	PlayerInputComponent->BindAction(TEXT("1_Key"), IE_Pressed, this, &AShooterCharacter::WeaponOneKeyPressed); //Binding item pickup action
	PlayerInputComponent->BindAction(TEXT("2_Key"), IE_Pressed, this, &AShooterCharacter::WeaponTwoKeyPressed); //Binding item pickup action
	PlayerInputComponent->BindAction(TEXT("3_Key"), IE_Pressed, this, &AShooterCharacter::WeaponThreeKeyPressed); //Binding item pickup action

//Sets Weapon mode
	PlayerInputComponent->BindAction(TEXT("WeaponMode"), IE_Pressed, this, &AShooterCharacter::SetWeaponMode); //Binding item pickup action

}

void AShooterCharacter::IncrementOverlappedItemCount(int8 amount)
{
	if (overlappedItemCount + amount <= 0)
	{
		overlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		overlappedItemCount += amount;
		bShouldTraceForItems = true;
	}
}

FVector AShooterCharacter::GetCameraInterpLocation(AItem* item)
{
	const FVector cameraWorldLocation(GetFollowCamera()->GetComponentLocation());
	const FVector cameraForwardDirection = GetFollowCamera()->GetForwardVector();
	const FVector cameraUpwardDirection = GetFollowCamera()->GetUpVector();

	FVector interpLocation;
	if (Cast<AWeapon>(item))
	{
		interpLocation = { cameraWorldLocation + (cameraForwardDirection * cameraWeaponInterpDistance) + (cameraUpwardDirection * cameraWeaponInterpElevation) };
	}
	else if (Cast<AAmmo>(item))
	{
		interpLocation = { cameraWorldLocation + (cameraForwardDirection * cameraAmmoInterpDistance) + (cameraUpwardDirection * cameraAmmoInterpElevation) };
	}

	return interpLocation;
}

void AShooterCharacter::GetPickupItem(AItem* item)
{
	if (item->GetEquipSoundCue())
	{
		UGameplayStatics::PlaySound2D(this, item->GetEquipSoundCue());
	}

	AWeapon* weapon = Cast<AWeapon>(item);
	if (weapon)
	{
		if (weapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{

			if (pistol)
			{
				if (pistol == equippedWeapon)
				{
					DropPistol();
					pickPistol(weapon);
					EquipPistol();
				}
				else
				{
					DropPistol();
					pickPistol(weapon);

				}
			}
			else
			{
				pickPistol(weapon);
			}
		}
		else if (gunsInventory.Num() < GUNS_INVENTORY_CAPACITY)
		{
			weapon->SetslotIndex(gunsInventory.Num());
			gunsInventory.Add(weapon);
			weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else //Inventory is Full
		{
			SwapWeapon(weapon);

		}
	}

	AAmmo* ammo = Cast<AAmmo>(item);
	if (ammo)
	{
		PickupAmmo(ammo);
	}
}

//Protected

void AShooterCharacter::MoveForward(float value)
{
	if (Controller && (value))
	{
		//Finding forward direction
		const FRotator rotation{ Controller->GetControlRotation() };
		const FRotator yawRotation{ 0,rotation.Yaw,0 };

		const FVector direction{ FRotationMatrix {yawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(direction, value);
	}
}

void AShooterCharacter::MoveRight(float value)
{
	if (Controller && (value))
	{
		//Finding right direction
		const FRotator rotation{ Controller->GetControlRotation() };
		const FRotator yawRotation{ 0,rotation.Yaw,0 };

		const FVector direction{ FRotationMatrix {yawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(direction, value);
	}
}

void AShooterCharacter::TurnAtRate(float value)
{
	float rate = value * baseTurnRate;
	APawn::AddControllerYawInput(rate);
}

void AShooterCharacter::LookAtRate(float value)
{
	float rate = value * baseLookUpRate;
	APawn::AddControllerPitchInput(rate);
}

void AShooterCharacter::jump()
{
	if (bCrouching)
	{
		bCrouching = !bCrouching;
		GetCharacterMovement()->MaxWalkSpeed = baseMovementSpeed;
		GetCharacterMovement()->GroundFriction = standingFriction;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::StopJump()
{
	ACharacter::StopJumping();
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if(bCrouching)
	{
		//Setting crouch walk speed
		GetCharacterMovement()->MaxWalkSpeed = crouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = crouchFriction;
	}
	else
	{
		//Setting crouch walk speed
		GetCharacterMovement()->MaxWalkSpeed = baseMovementSpeed;
		GetCharacterMovement()->GroundFriction = standingFriction;
	}
}

void AShooterCharacter::FireWeapon()
{
	if (!equippedWeapon) return;
	if (combactState != ECombactState::ECS_UNOccupied) return;
	if (WeaponHasAmmo())
	{
		//Play fire sound
		PlayFireSound();

		//Play fire hip animination
		PlayGunHipFireMontage();

		//fire the bullet from gun
		SendBullet();

		//decremt ammo
		equippedWeapon->DecrementAmmo();

		//Shooting bullet crosshair spread
		StartBulletFireCrosshairSpread();

		//Do this for auto guns --- for single guns we dont need to do this
		if (equippedWeapon->GetISWeaponAuto())
		{
			StartFireTimer();
		}
		else
		{
			if (!WeaponHasAmmo())
				ReloadWeapon();
		}
	}
}

void AShooterCharacter::AimON()
{
	if (GetPlayerCombatState() == ECombactState::ECS_Reload) return;
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = aimMovementSpeed;

	//Setting Aim Sensitivity
	baseTurnRate = aimTurnRate;
	baseLookUpRate = aimLookUpRate;
}

void AShooterCharacter::AimOff()
{
	bAiming = false;
	
	bCrouching ? GetCharacterMovement()->MaxWalkSpeed = crouchMovementSpeed : GetCharacterMovement()->MaxWalkSpeed = baseMovementSpeed;

	//Setting Aim Sensitivity
	baseTurnRate = hipTurnRate;
	baseLookUpRate = hipLookUpRate;
}

void AShooterCharacter::AimingButtonFlipFlop()
{
	if (bAiming) { AimOff(); }
	if (!bAiming) { AimON(); }
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (!equippedWeapon) return;
	combactState = ECombactState::ECS_FireTimerInProgress;

	//Call autofire reset
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, equippedWeapon->GetFireRate());
}

void AShooterCharacter::AutoFireReset()
{
	combactState = ECombactState::ECS_UNOccupied;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		//Reload the weapon
		ReloadWeapon();
	}
}

void AShooterCharacter::PlayFireSound()
{
	//Play Fire sound
	if (equippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, equippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	//Muzzle Flash
	const USkeletalMeshSocket* barrelSocket = equippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (barrelSocket)
	{
		const FTransform socketTransform = barrelSocket->GetSocketTransform(equippedWeapon->GetItemMesh());

		if (equippedWeapon->GetMuzzleFlash())
		{
			//Spawn particles
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), equippedWeapon->GetMuzzleFlash(), socketTransform);
		}
	}

	//Impact Particles effect

	//Converting crosshair location into world location
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		//Get current size of the viewport
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	//Get crosshair location
	FVector2D crosshairLocation{ viewportSize.X / 2 , viewportSize.Y / 2 };
	FVector crosshairWorldPostion;
	FVector crosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldPostion, crosshairWorldDirection);

	if (bScreenToWorld)	//If conversion from viewport to world is successful
	{
		//Params for line trace
		FHitResult fireHit;
		FVector startLocation{ crosshairWorldPostion };
		FVector endLocation{ startLocation + crosshairWorldDirection * 50'000.f };

		//Line tracing
		GetWorld()->LineTraceSingleByChannel(fireHit, startLocation, endLocation, ECollisionChannel::ECC_Visibility);
		if (fireHit.bBlockingHit)	//if line trace is successful -- Strikes something
		{
			if (impactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, impactParticle, fireHit.Location);
			}
		}
	}
}

void AShooterCharacter::PlayGunHipFireMontage()
{
	//Play fire hip animination
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && hipFireMontage)
	{
		animInstance->Montage_Play(hipFireMontage);
		animInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (combactState != ECombactState::ECS_UNOccupied) return;
	if (!equippedWeapon) return;
	if (equippedWeapon->GetAmmo() == equippedWeapon->GetMagazineCapacity()) return;

	//If aiming -- get out of it
	if (bAiming)
	{
		AimOff();
	}

	if (CarryingAmmo())//Do we carrying ammo of weapon type
	{
		combactState = ECombactState::ECS_Reload;
		//Play reloading Anim

		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (reloadMontage && animInstance)
		{
			animInstance->Montage_Play(reloadMontage);
			
			FName montageSection =  equippedWeapon->GetReloadMontageSectionName();
			animInstance->Montage_JumpToSection(montageSection);
		}
	}
}

void AShooterCharacter::StopReloadingWeapon()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (reloadMontage && animInstance)
	{
		animInstance->Montage_Stop(0.1f, reloadMontage);
	}

	combactState = ECombactState::ECS_UNOccupied;
}

void AShooterCharacter::FinishReloading()
{
	//Finish reloading
	combactState = ECombactState::ECS_UNOccupied;

	if (!equippedWeapon) return;

	const EAmmoType weaponAmmoType = equippedWeapon->GetAmmoType();
	if (ammoMap.Contains(weaponAmmoType))
	{
		//Amonut of ammo we have before reloading finished
		int32 carriedAmmo = ammoMap[weaponAmmoType];

		//capacity left inside magazine
		const int32 magSpace = equippedWeapon->GetMagazineCapacity() - equippedWeapon->GetAmmo();

		if (magSpace > carriedAmmo)
		{
			//Reload the magazine with all ammo we are carring
			equippedWeapon->reloadAmmo(carriedAmmo);
			carriedAmmo = 0;
			//Update ammoMap
			ammoMap.Add(weaponAmmoType, carriedAmmo);
		}
		else
		{
			//Fill the magazine
			equippedWeapon->reloadAmmo(magSpace);
			carriedAmmo -= magSpace;
			//Update ammoMap
			ammoMap.Add(weaponAmmoType, carriedAmmo);
		}
	}
	
}

void AShooterCharacter::FinishEquipping()
{
	combactState = ECombactState::ECS_UNOccupied;
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	//when aiming happens ---  Setting CurrentFOV value
	if (bAiming)
	{
		//Setting Current FOV --- Interpolate to Zoomed FOV
		cameraCurrentFOV = FMath::FInterpTo(cameraCurrentFOV, cameraZoomedFOV, DeltaTime, zoomInterpSpeed);
	}
	else
	{
		//Setting Current FOV --- Interpolate to Default FOV
		cameraCurrentFOV = FMath::FInterpTo(cameraCurrentFOV, cameraDefaultFOV, DeltaTime, zoomInterpSpeed);
	}

	//Setting Camera FOV to current FOV value
	GetFollowCamera()->SetFieldOfView(cameraCurrentFOV);
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
//Setting crosshairVelocity Factor
	FVector2D walkSpeed{ 0.f, 600.f };
	FVector2D velocityMultiplierRange{ 0.f, 1.f };
	FVector velocity = GetVelocity();
	velocity.Z = 0;

	crosshairVelocityFactor = FMath::GetMappedRangeValueClamped(walkSpeed, velocityMultiplierRange, velocity.Size());

//Setting crosshairAir Factor
	if (GetCharacterMovement()->IsFalling())//In Air
	{
		//Spread crosshair
		crosshairInAirFactor = FMath::FInterpTo(crosshairInAirFactor, 1.f, DeltaTime, 2.25f);
	}
	else
	{
		//Shrink crosshair
		crosshairInAirFactor = FMath::FInterpTo(crosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

//Setting Aim factor
	if (bAiming)
	{
		crosshairAimFactor = FMath::FInterpTo(crosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		crosshairAimFactor = FMath::FInterpTo(crosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

//Fire response
	if (bBulletFire)
	{
		crossShootFactor = FMath::FInterpTo(crossShootFactor, 0.2f, DeltaTime, 60.f);
	}
	else
	{
		crossShootFactor = FMath::FInterpTo(crossShootFactor, 0.f, DeltaTime, 60.f);
	}

	//Setting crosshairSpread
	crooshairSpreadMultiplier = 0.5f + crosshairVelocityFactor + crosshairInAirFactor - crosshairAimFactor + crossShootFactor; // Base value 0.5 + crosshairMultiplier + airfactor
	
}

void AShooterCharacter::StartBulletFireCrosshairSpread()
{
	bBulletFire = true;

	GetWorldTimerManager().SetTimer(crosshairShootTimer, this, &AShooterCharacter::FinishBulletFireCrosshairSpread, shootTimeDuration);
}

void AShooterCharacter::FinishBulletFireCrosshairSpread()
{
	bBulletFire = false;
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& outHitResult)
{
	//Getting viewport Size
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	FVector2D crosshairLocation{ viewportSize.X / 2 , viewportSize.Y / 2 };
	FVector crosshairWorldPostion;
	FVector crosshairWorldDirection;

	//Screen to World Position
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldPostion, crosshairWorldDirection);

	if (bScreenToWorld)	//If conversion from viewport to world is successful
	{
		//Params for line trace
		FHitResult fireHit;
		FVector startLocation{ crosshairWorldPostion };
		FVector endLocation{ startLocation + crosshairWorldDirection * 50'000.f };

		//Line tracing
		GetWorld()->LineTraceSingleByChannel(outHitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility);

		if (outHitResult.bBlockingHit)
		{
			return true;
		}
	}

	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult itemHitResult;
		TraceUnderCrosshairs(itemHitResult);
		if (itemHitResult.bBlockingHit)
		{
			traceHitItem = Cast<AItem>(itemHitResult.Actor);

			if ((traceHitItem) && (traceHitItem->GetItemSate() == EItemState::EIS_EquipInterping))
			{
				traceHitItem = nullptr;
			}

			if (traceHitItem && traceHitItem->GetPickupWidget())
			{
				traceHitItem->GetPickupWidget()->SetVisibility(true);
				traceHitItem->GetItemMesh()->SetRenderCustomDepth(true);
			}

			if (TraceHitLastFrame)
			{
				if (traceHitItem != TraceHitLastFrame)
				{
					//We are hitting a different Item
					TraceHitLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitLastFrame->GetItemMesh()->SetRenderCustomDepth(false);
				}
			}
			//Store a refrence to the hitItem -- to close the widget when we move away
			TraceHitLastFrame = traceHitItem;
		}
	}
	else if(TraceHitLastFrame)
	{
		//No longer overlapping any item
		TraceHitLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitLastFrame->GetItemMesh()->SetRenderCustomDepth(false);
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (defaultWeaponClass)
	{
		//Spawn the actor
		return GetWorld()->SpawnActor<AWeapon>(defaultWeaponClass);
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* weaponToEquip)
{
	if (weaponToEquip)
	{
		//Get the right hand socket
		const USkeletalMeshSocket* handSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handSocket)
		{
			//Attach the default wepon to the hand socket
			handSocket->AttachActor(weaponToEquip, GetMesh());
		}

		if (equippedWeapon == nullptr)
		{
			//-1 == no equipped weapon
			equipItemDelegate.Broadcast(-1, weaponToEquip->GetslotIndex());
		}
		else
		{
			equipItemDelegate.Broadcast(equippedWeapon->GetslotIndex(), weaponToEquip->GetslotIndex());
		}

		//Set equipped weapon
		equippedWeapon = weaponToEquip;
		equippedWeapon->SetItemState(EItemState::EIS_Equipped);
		weaponToEquip->SetWeaponOwner(this);


		//Reload the weapon if it have zero ammo
		if (equippedWeapon->GetAmmo() == 0)
		{
			if (ammoMap.Find(equippedWeapon->GetAmmoType()))
			{
				ReloadWeapon();
			}
		}
	}
}

void AShooterCharacter::DropWeapon()
{
	if (equippedWeapon)
	{
		FDetachmentTransformRules detachmentTransformRules(EDetachmentRule::KeepWorld, true);
		equippedWeapon->GetItemMesh()->DetachFromComponent(detachmentTransformRules);

		//Setting item state to falling
		equippedWeapon->SetItemState(EItemState::EIS_Falling);
		equippedWeapon->ThrowWeapon();
		equippedWeapon = nullptr;
	}
}

void AShooterCharacter::DropPistol()
{
	if (!pistol) return;

	FDetachmentTransformRules detachmentTransformRules(EDetachmentRule::KeepWorld, true);
	pistol->GetItemMesh()->DetachFromComponent(detachmentTransformRules);

	pistol->GetItemMesh()->SetVisibility(true);
	pistol->SetItemState(EItemState::EIS_Falling);
	pistol->ThrowWeapon();
	pistol = nullptr;
}

void AShooterCharacter::pickPistol(AWeapon* weapon)
{
	//making pistol to point the weapon -- pistol
	pistol = weapon;
	pistol->SetWeaponOwner(this);
	
	const USkeletalMeshSocket* handSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handSocket)
	{
		//Attach the default wepon to the hand socket
		handSocket->AttachActor(weapon, GetMesh());
	}
	pistol->SetItemState(EItemState::EIS_PickedUp);

}

void AShooterCharacter::EquipPistol()
{
	if (!pistol) return;

	equippedWeapon = pistol;
	pistol->SetItemState(EItemState::EIS_Equipped);

	//Reload the weapon if it have zero ammo
	if (pistol->GetAmmo() == 0)
	{
		if (ammoMap.Find(pistol->GetAmmoType()))
		{
			ReloadWeapon();
		}
	}

}

void AShooterCharacter::weaponRecovery(AWeapon* weaponToEquip)
{
	if (!pistol) return;

	pistol->SetItemState(EItemState::EIS_PickedUp);

	equippedWeapon = weaponBeforePistol;
	equippedWeapon->SetItemState(EItemState::EIS_Equipped);

	if (weaponBeforePistol == weaponToEquip)
	{
		//Equip anim
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance && equipMontage)
		{
			animInstance->Montage_Play(equipMontage);
			animInstance->Montage_JumpToSection(FName("Equip"));
		}

		//Play equip Sound
		if (equippedWeapon && equippedWeapon->GetEquipSoundCue())
		{
			UGameplayStatics::PlaySound2D(this, equippedWeapon->GetEquipSoundCue());

		}

		if (bAiming)
		{
			AimOff();
		}
	}

	weaponBeforePistol = nullptr;
}

void AShooterCharacter::TakePistol()
{
	if (!pistol) return;
	if (equippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol) return;


	weaponBeforePistol = equippedWeapon;
	equippedWeapon->SetItemState(EItemState::EIS_PickedUp);

	equippedWeapon = pistol;
	pistol->SetItemState(EItemState::EIS_Equipped);

	
	//Play equip weapon for pistol
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && equipMontage)
	{
		animInstance->Montage_Play(equipMontage);
		animInstance->Montage_JumpToSection(FName("Equip"));
	}

	//Play equip Sound
	if (equippedWeapon && equippedWeapon->GetEquipSoundCue())
	{
		UGameplayStatics::PlaySound2D(this, equippedWeapon->GetEquipSoundCue());
	}

	if (bAiming)
	{
		AimOff();
	}
}

void AShooterCharacter::ItemPickupAndDropButtonPressed()
{
	if (combactState != ECombactState::ECS_UNOccupied) return;

	//Enable liretrace to pickup other items while picking one item
	bIsWeaponSwapping = true;

	if (traceHitItem)
	{
		//AWeapon* traceHitWeapon = Cast<AWeapon>(traceHitItem);
		traceHitItem->StartItemCurve(this);
		traceHitItem = nullptr;

		if (bAiming)
		{
			AimOff();
		}
	}
	
	//Enable liretrace to pickup other items while picking one item
	bIsWeaponSwapping = false;
}

void AShooterCharacter::ItemPickupAndDropButtonReleased()
{
}

void AShooterCharacter::SwapWeapon(AWeapon* weaponToSwap)
{

	if (gunsInventory.Num() - 1 >= equippedWeapon->GetslotIndex())
	{
		gunsInventory[equippedWeapon->GetslotIndex()] = weaponToSwap;
		weaponToSwap->SetslotIndex(equippedWeapon->GetslotIndex());
	}

	DropWeapon();
	EquipWeapon(weaponToSwap);
	traceHitItem = nullptr;
	TraceHitLastFrame = nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
	ammoMap.Add(EAmmoType::EAM_9, starting9mm);
	ammoMap.Add(EAmmoType::EAM_556, starting556mm);
	ammoMap.Add(EAmmoType::EAM_762, starting762mm);
	ammoMap.Add(EAmmoType::EAM_SHELL, startingShells);

}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (equippedWeapon == nullptr) return false;
	
	if(equippedWeapon->GetAmmo()) return  true;

	return false;
}

bool AShooterCharacter::CarryingAmmo()
{
	if (equippedWeapon)
	{
		EAmmoType weaponAmmoType = equippedWeapon->GetAmmoType();
		
		if (ammoMap.Contains(weaponAmmoType))
		{
			return ammoMap[weaponAmmoType] > 0;
		}
	}
	return false;
}

void AShooterCharacter::GrabClip()
{
	if (!equippedWeapon) return;
	if (!handSceneComponent) return;

	//Get clipbone index

	int32 clipBoneIndex{ equippedWeapon->GetItemMesh()->GetBoneIndex( equippedWeapon->GetClipBoneName()) };
	if(clipBoneIndex)
	clipTransform = equippedWeapon->GetItemMesh()->GetBoneTransform(clipBoneIndex);

	FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, true);
	handSceneComponent->AttachToComponent(GetMesh(), attachmentRules, "hand_l");
	handSceneComponent->SetWorldTransform(clipTransform);

	equippedWeapon->SetbMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	equippedWeapon->SetbMovingClip(false);
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float targetCapsuleHalfHeight;
	bCrouching ? targetCapsuleHalfHeight = crouchingCapsuleHalfHeight : targetCapsuleHalfHeight = standingCapsuleHalfHeight;

	currentCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float interpHalfHeight{ FMath::FInterpTo(currentCapsuleHalfHeight, targetCapsuleHalfHeight, DeltaTime,10.f) };

	//-ve while crouching +ve while standing
	const float deltaCapsuleHalfHeight{ interpHalfHeight - currentCapsuleHalfHeight };

	FVector meshoffSet{ 0.f, 0.f, -deltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(meshoffSet);
	GetCameraBoom()->AddLocalOffset(meshoffSet* (-0.2f));

	GetCapsuleComponent()->SetCapsuleHalfHeight(interpHalfHeight);

}

void AShooterCharacter::PickupAmmo(AAmmo* ammo)
{
	//Ammo updating
	if (ammoMap.Find(ammo->GetAmmoType()))
	{
		int32 tempAmmo;
		tempAmmo = ammoMap[ammo->GetAmmoType()] + ammo->GetItemCount();
		ammoMap[ammo->GetAmmoType()] = tempAmmo;
	}

	//if equipped wepon of same type ammo hae zeo bullets then reload
	if (equippedWeapon->GetAmmoType() == ammo->GetAmmoType() && equippedWeapon->GetAmmo() == 0)
	{
		ReloadWeapon();
	}

	ammo->Destroy();
}

void AShooterCharacter::WeaponOneKeyPressed()
{
	if (equippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
	{
		if(gunsInventory.Num() >= 1)
			weaponRecovery(Cast<AWeapon>(gunsInventory[0]));
	}

	if (equippedWeapon->GetslotIndex() == 0) return;
	ExchangeInventoryItems(equippedWeapon->GetslotIndex(), 0);
	

}

void AShooterCharacter::WeaponTwoKeyPressed()
{
	if (equippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
	{
		if (gunsInventory.Num() >= 2)
			weaponRecovery(Cast<AWeapon>(gunsInventory[1]));
	}
	
	if (equippedWeapon->GetslotIndex() == 1) return;
	ExchangeInventoryItems(equippedWeapon->GetslotIndex(), 1);
	
}

void AShooterCharacter::WeaponThreeKeyPressed()
{
	if (pistol)
		TakePistol();
}

void AShooterCharacter::ExchangeInventoryItems(int32 currentItemIndex, int32 newItemIndex)
{

	if ((currentItemIndex == newItemIndex) || (newItemIndex >= gunsInventory.Num())) return;

	StopReloadingWeapon();

	auto oldEquipedWeapon = equippedWeapon;
	auto newWeapon = Cast<AWeapon>(gunsInventory[newItemIndex]);

	EquipWeapon(newWeapon);

	oldEquipedWeapon->SetItemState(EItemState::EIS_PickedUp);
	newWeapon->SetItemState(EItemState::EIS_Equipped);

	combactState = ECombactState::ECS_Equipping;

	//Equip anim
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && equipMontage)
	{
		animInstance->Montage_Play(equipMontage);
		animInstance->Montage_JumpToSection(FName("Equip"));
	}

	//Play equip Sound
	if (equippedWeapon && equippedWeapon->GetEquipSoundCue())
	{
		UGameplayStatics::PlaySound2D(this, equippedWeapon->GetEquipSoundCue());

	}

	if (bAiming)
	{
		AimOff();
	}
}

void AShooterCharacter::SetWeaponMode()
{
	if (!equippedWeapon) return;


	if (equippedWeapon->GetISWeaponAuto())
		equippedWeapon->SetWeaponMode(false);
	else
		equippedWeapon->SetWeaponMode(true);
}

