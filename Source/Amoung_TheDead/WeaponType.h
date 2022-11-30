#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubMachineGun UMETA(DisplayName = "Sub machine Gun"),
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_SingleSot UMETA(DisplayName = "Sniper Riflr"),
	EWT_ShotGun UMETA(DisplayName = "Shot Gun"),

	EWT_Pistol UMETA(DisplayName = "Pistol"),

	EWT_MAX UMETA(DisplayName = "Default Max"),

};