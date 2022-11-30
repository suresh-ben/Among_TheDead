// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"

#include "Weapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* itemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString itemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType ammoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 weaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 magzineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* pickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* equipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* weaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ammoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName clipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float autoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* muzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* fireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName bonToHide;
};

UCLASS()
class AMOUNG_THEDEAD_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void OnConstruction(const FTransform& transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:
	//variables


private:
	//variables
	FTimerHandle throwWeaponTimerHandle;
	float throwWeaponTime;
	bool bFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float weaponThrowImpulse;

	/* The owner or the player holding the gun (can be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* weaponOwner;

	/* ammo inside the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ammo;

	/* Max bullets in a magazine */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 magazineCapacity;

	/* the type of the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType weaponType;

	/* The typr of ammo that can be used by this gun */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType ammoType;

	/* Name tag of the reload animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FName reloadMontageSectionName;
 
//Clip Anim section

	/* True when clip is moving while reloading */
	bool bMovingClip;
	/* name of the clip bone */
	FName clipBoneName;

//Auto vs Single bullets

	/* true when gun is in auto mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsAuto;

	/* Data table for Weapon Properties */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UDataTable* weaponDataTable;

	/* fire rate of the weapon for auto mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float autoFireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* muzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* fireSound;

	/* For pistol */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FName boneToHide;

//item roation Correction

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 SingleShotCorrectionFactor;

public:
	//Getters
	
	FORCEINLINE AShooterCharacter* GetWeaponOwner() const { return weaponOwner; }
	FORCEINLINE int32 GetAmmo() const { return ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return magazineCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return weaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return ammoType; }
	FORCEINLINE FName GetReloadMontageSectionName() const { return reloadMontageSectionName; }
	FORCEINLINE FName GetClipBoneName() const { return clipBoneName; }
	UFUNCTION(BlueprintCallable)
	bool GetbMovingClip() const { return bMovingClip; }
	FORCEINLINE bool GetISWeaponAuto() const { return bIsAuto; }
	FORCEINLINE float GetFireRate() const { return autoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return muzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return fireSound; }

	//Setters
	FORCEINLINE void SetReloadMontageSection(FName montageName) { reloadMontageSectionName = montageName; }
	FORCEINLINE void SetClipBoneName(FName boneName) { clipBoneName = boneName; }
	FORCEINLINE void SetWeaponOwner(AShooterCharacter* shooterCharacter) { weaponOwner = shooterCharacter; }
	FORCEINLINE void SetbMovingClip(bool b) { bMovingClip = b; }
	FORCEINLINE void SetWeaponMode(bool boolean) { bIsAuto = boolean; }
	FORCEINLINE void SetAmmo(int32 num) { ammo = num; }

	/* Will be called by player when fire button is pressed */
	void DecrementAmmo();

	//Functions

	/* Adds impusle and throuws the weapon from player hands */
	void ThrowWeapon();

	//Functions

	//Set Ammo
	void reloadAmmo(int32 amount);

protected:
	//Functions

	void StopFalling();

	
};
