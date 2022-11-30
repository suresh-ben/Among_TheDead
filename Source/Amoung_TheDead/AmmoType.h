#pragma once

//Bullets --- types
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAM_9 UMETA(DisplayName = "SMG"),
	EAM_556 UMETA(DisplayName = "Assualt Rifle"),
	EAM_762 UMETA(DisplayName = "Sniper Rifle"),
	EAM_SHELL UMETA(DisplayName = "ShotGun"),

	EXM_MAX UMETA(DisplayName = "Deafult Max")
};