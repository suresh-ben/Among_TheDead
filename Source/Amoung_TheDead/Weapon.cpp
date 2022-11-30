// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "ShooterCharacter.h"

AWeapon::AWeapon() :
	throwWeaponTime(1.f),
	bFalling(false),
	weaponThrowImpulse(10'000.f),
	weaponType(EWeaponType::EWT_SubMachineGun),
	ammoType(EAmmoType::EAM_9),
	reloadMontageSectionName(FName(TEXT("ReloadSMG"))),
	magazineCapacity(30),
	bMovingClip(false),
	clipBoneName(FName(TEXT("smg_clip"))),
	bIsAuto(true),
	SingleShotCorrectionFactor(90)
{
	PrimaryActorTick.bCanEverTick = true;

	//Initialization
	ammo = magazineCapacity;

}

void AWeapon::OnConstruction(const FTransform& transform)
{
	const FString weaponTablePath{ TEXT("DataTable'/Game/_Game/DataTables/WeaponDataTable.WeaponDataTable'") };
	UDataTable* weaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *weaponTablePath));

	if (weaponTableObject)
	{
		FWeaponDataTable* weaponDataRow = nullptr;

		switch (weaponType)
		{
		case EWeaponType::EWT_SubMachineGun: //Ares

			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("Ares")), TEXT(""));

			break;

		case EWeaponType::EWT_AssaultRifle: //Phantom

			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("Phantom")), TEXT(""));

			break;

		case EWeaponType::EWT_SingleSot: //BullDog

			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("BullDog")), TEXT(""));

			break;

		case EWeaponType::EWT_ShotGun: //Judge

			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("Judge")), TEXT(""));

			break;

		case EWeaponType::EWT_Pistol : //Scorpio

			weaponDataRow = weaponTableObject->FindRow<FWeaponDataTable>(FName(TEXT("Scorpio")), TEXT(""));

			break;
		}

		if (weaponDataRow)
		{
			GetItemMesh()->SetSkeletalMesh(weaponDataRow->itemMesh);
			SetItemName(weaponDataRow->itemName);

			ammoType = weaponDataRow->ammoType;
			ammo = weaponDataRow->weaponAmmo;
			magazineCapacity = weaponDataRow->magzineCapacity;

			SetPickupSound(weaponDataRow->pickupSound);
			SetEquipSound(weaponDataRow->equipSound);

			SetItemIcon(weaponDataRow->weaponIcon);
			SetAmmoIcon(weaponDataRow->ammoIcon);

			SetClipBoneName(weaponDataRow->clipBoneName);
			SetReloadMontageSection(weaponDataRow->ReloadMontageSection);
			GetItemMesh()->SetAnimInstanceClass(weaponDataRow->AnimBp);

			autoFireRate = weaponDataRow->autoFireRate;
			fireSound = weaponDataRow->fireSound;
			muzzleFlash = weaponDataRow->muzzleFlash;

			boneToHide = weaponDataRow->bonToHide;

		}

	}

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (boneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(boneToHide, EPhysBodyOp::PBO_None);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Do this if u want to have weapon up right after falling
	/*if (GetItemSate() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation( 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}*/
}

void AWeapon::DecrementAmmo()
{
	ammo--;
	if (ammo <= 0)
	{
		ammo = 0;
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation;
	if (weaponOwner)
	{
		MeshRotation = FRotator(25.f, weaponOwner->GetViewRotation().Yaw - 70.f, 25.f);
	}
	else
	{
		MeshRotation = FRotator(25.f, GetItemMesh()->GetComponentRotation().Yaw, 25.f);
	}
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward(GetItemMesh()->GetForwardVector());
	const FVector MeshRight(GetItemMesh()->GetRightVector());

	//Direction in which we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(0.f, FVector(0.f, .1f, 1.f));
	ImpulseDirection *= weaponThrowImpulse;

	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(throwWeaponTimerHandle, this, &AWeapon::StopFalling, throwWeaponTime);

}

void AWeapon::reloadAmmo(int32 amount)
{
	checkf(ammo + amount <= magazineCapacity, TEXT("magazine capacity overloading"));
	ammo += amount;
}

void AWeapon::StopFalling()
{
	bFalling = false;

	//Setting weapon state to pickup
	SetItemState(EItemState::EIS_Pickup);

	//Setting Owner to null
	weaponOwner = nullptr;
}

