// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class AMOUNG_THEDEAD_API AAmmo : public AItem
{
	GENERATED_BODY()

public:

	AAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/* Ammo mesh for pickup */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ammoMesh;

	/* The type of the ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType ammoType;

	/* Pickup Auto */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ammoCollisionSphere;

public:
	//Getters
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return ammoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return ammoType; }

protected:

	virtual void SetItemProperties(EItemState state) override;
	
	UFUNCTION()
		void AmmoSphereCollisionOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

};
