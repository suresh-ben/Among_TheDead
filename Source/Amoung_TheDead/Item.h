// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

//Enums Section
UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "Deafult")
};

//class
UCLASS()
class AMOUNG_THEDEAD_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	virtual void OnConstruction(const FTransform& transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:


//widgetProperties

	/* Item name on pickup widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget-Item Properties", meta = (AllowPrivateAccess = "true"))
	FString itemName;

	/*  Item count (Ammo, etc...)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget-Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/* LineTrace collides with box to show HUD widgets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* collisionBox;

	/* Skeletal mesh for the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* itemMesh;

	/* Popup widget when th player looks at the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* pickupWidget;

	/* Enables Item tracing when overlapped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* aresSphere;

	/* State of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState itemState;

//When Equpping --- interpolation

	/* Z curve that will be follwed by the item while interpolating when player pickedup the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* itemZCurve;

	/* Z curve that will be follwed by the item while interpolating when player pickedup the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* itemScaleCurve;

	/* Starting location when interpolation begins */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector itemInterpStartLocation;

	/* Camera target location in front of camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector cameraTargetLocation;

	/* True when interping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bInterping;

	/* plays when we start interping */
	FTimerHandle itemInterpTimer;
	/* Time for interpolation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float zCurveTime;

	/* X and Y for the Item while interping in the equipping */
	float itemInterpX;
	float itemInterpY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* character;

//Sounds

	/* Sound played when item is pickedUp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* pickupSound;

	/* Sound played when item is Equipped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* equipSound;

//Glow effects

	/* Gun glow base effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* itemBase;

//Item ammo type --- icon

	/* the texture on the popup widget for ammo type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ammoIconTexture;

	/* Weapon icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* iconItem;

	/* weapon slot from gunsInventory array */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 slotIndex;

public:
	//Getters

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return pickupWidget; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return itemMesh; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return aresSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return collisionBox; }
	FORCEINLINE EItemState GetItemSate() const { return itemState; }
	FORCEINLINE USoundCue* GetPickupSoundCue() const { return pickupSound; }
	FORCEINLINE USoundCue* GetEquipSoundCue() const { return equipSound; }
	FORCEINLINE UStaticMeshComponent* GetItemBase() const { return itemBase; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE int32 GetslotIndex() const { return slotIndex; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetNameOftheItem() const { return itemName; }

	//Setters
	FORCEINLINE void SetPickupSound(USoundCue* Sound) { pickupSound = Sound; }
	FORCEINLINE void SetEquipSound(USoundCue* Sound) { equipSound = Sound; }
	FORCEINLINE void SetItemName(FString name) { itemName = name; }
	FORCEINLINE void SetItemIcon(UTexture2D* icon) { iconItem = icon; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* icon) { ammoIconTexture = icon; }

	void SetItemState(EItemState state);
	FORCEINLINE void SetslotIndex(int32 index) { slotIndex = index; }

	//Functions

	/* Called from AShooterCharcter class */
	void StartItemCurve(AShooterCharacter* shooter);
	/* call back after interpolating */
	void FinishInterp();

protected:

//Functions
	/* Called when area sphere gets overlappes */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult &sweepResult);
	
	/* Called when overlapping ends */
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex);

	/* Ses item properties for a sate*/
	virtual void SetItemProperties(EItemState state);

	/* Handels item interpolation when in the eupping state */
	void ItemInterp(float DeltaTime);

	//Glow Effect
	void EnableCustomDepth();
	void DisableCustomDepth();
	
};
