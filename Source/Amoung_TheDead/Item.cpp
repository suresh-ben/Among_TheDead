// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AItem::AItem():
	itemName(FString("Default")),
	itemState(EItemState::EIS_Pickup),
	//Item interpolation --- Equpping
	zCurveTime(.7f),
	itemInterpStartLocation(FVector(0.f)),
	itemInterpX(0.f),
	itemInterpY(0.f),
	cameraTargetLocation(FVector(0.f)),
	bInterping(false),
	slotIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creting mesh
	itemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	SetRootComponent(itemMesh);

	//Creating collision Box
	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collison Box"));
	collisionBox->SetupAttachment(itemMesh);
	collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	//Creating Pickup widget
	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	pickupWidget->SetupAttachment(RootComponent);

	//Creating Area sphere
	aresSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	aresSphere->SetupAttachment(itemMesh); 
	//aresSphere->SetSphereRadius(100.f);

	//Creating item Base
	itemBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun Base"));
	itemBase->SetupAttachment(itemMesh);

	OnConstruction(this->GetTransform());
}

void AItem::OnConstruction(const FTransform& transform)
{
	//Will be redifined in weapon and ammo classes
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	//Hide Pickup Widget
	pickupWidget->SetVisibility(false);
	itemBase->SetVisibility(false);

	//Setup overlap for Area sphere
	aresSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	aresSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//Setting default properties of item on startup project
	SetItemProperties(itemState);

	//Set Glow effect to false
	itemMesh->SetRenderCustomDepth(false);

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handels item interpolation when in the eupping state 
	ItemInterp(DeltaTime);

}

void AItem::SetItemState(EItemState state)
{
	//setting item state
	itemState = state;
	//setting item properties
	SetItemProperties(itemState);

}

void AItem::StartItemCurve(AShooterCharacter* shooter)
{
	//Setting the charcter ref 
	character = shooter;

	if (pickupSound)
	{
		UGameplayStatics::PlaySound2D(this, pickupSound);
	}

	//interpolation start point -- Location
	itemInterpStartLocation = GetActorLocation();
	bInterping = true;
	//Setting item state to inetrpng
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(itemInterpTimer, this, &AItem::FinishInterp, zCurveTime);

}

void AItem::FinishInterp()
{
	bInterping = false;
	if (character)
	{
		character->GetPickupItem(this);
	}
	SetActorScale3D(FVector(1.f));
}

void AItem::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult &sweepResult)
{
	if (otherActor)
	{
		AShooterCharacter* shooter = Cast<AShooterCharacter>(otherActor);
		if (shooter)
		{
			shooter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex)
{
	if (otherActor)
	{
		AShooterCharacter* shooter = Cast<AShooterCharacter>(otherActor);
		if (shooter)
		{
			shooter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetItemProperties(EItemState state)
{
	switch (state)
	{
		case EItemState::EIS_Pickup :

			//Set mesh properties
			itemMesh->SetSimulatePhysics(false);
			itemMesh->SetEnableGravity(true);
			itemMesh->SetVisibility(true);
			itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//Area sphere properties
			aresSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			aresSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//collision box properties
			collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			break;

		case EItemState::EIS_EquipInterping :

			pickupWidget->SetVisibility(false);

			//Set mesh properties
			itemMesh->SetSimulatePhysics(false);
			itemMesh->SetEnableGravity(false);
			itemMesh->SetVisibility(true);
			itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//Area sphere properties
			aresSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			aresSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//collision box properties
			collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

		case EItemState::EIS_PickedUp:

			pickupWidget->SetVisibility(false);

			//Set mesh properties
			itemMesh->SetSimulatePhysics(false);
			itemMesh->SetEnableGravity(false);
			itemMesh->SetVisibility(false);
			itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//Area sphere properties
			aresSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			aresSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//collision box properties
			collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

		case EItemState::EIS_Equipped :
			//Set pickupwidget of this weapon to hide
			GetPickupWidget()->SetVisibility(false);

			//Set mesh properties
			itemMesh->SetSimulatePhysics(false);
			itemMesh->SetEnableGravity(false);
			itemMesh->SetVisibility(true);
			itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//Area sphere properties
			aresSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			aresSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//collision box properties
			collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

		case EItemState::EIS_Falling :

			//Set mesh properties
			itemMesh->SetSimulatePhysics(true);
			itemMesh->SetEnableGravity(true);
			itemMesh->SetVisibility(true);
			itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			itemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

			//Area sphere properties
			aresSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			aresSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//collision box properties
			collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			break;

	}

}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;
	if (character && itemZCurve)
	{
	//Interpolating Z location
		//Elapsed time since we started the timer
		const float elapsedTime = GetWorldTimerManager().GetTimerElapsed(itemInterpTimer);
		//Get curve value corresponding to the time passed
		const float curveValueOnGraph = itemZCurve->GetFloatValue(elapsedTime);

		//loaction setting
		FVector itemLocation = itemInterpStartLocation;
		//setting target location infront of camera
		const FVector cameraInterpLocation{ character->GetCameraInterpLocation(this) };

		//vector from item to camera interp location. Z magnitude
		const FVector itemToCamera{ FVector(0.f, 0.f, (cameraInterpLocation - itemLocation).Z) };
		//Scale factor for ZCurve
		const float deltaZ = itemToCamera.Size();
		

	//Interpolating X and Y loactions
		const FVector currentLocation( GetActorLocation() );
		//Interping X
		const float interpX = FMath::FInterpTo(currentLocation.X, cameraInterpLocation.X, DeltaTime, 30.f);
		//Interping Y
		const float interpY = FMath::FInterpTo(currentLocation.Y, cameraInterpLocation.Y, DeltaTime, 30.f);

		//Set the X and Y itemLocation
		itemLocation.X = interpX;
		itemLocation.Y = interpY;

		//Set the Z itemLocation
		itemLocation.Z += deltaZ * curveValueOnGraph;

		//setting mesh location
		SetActorLocation(itemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		//Setting rotation
		FRotator cameraRotation = FRotator(0.f, character->GetFollowCamera()->GetComponentRotation().Yaw, 0.f);
		if(cameraRotation.Yaw != GetActorRotation().Yaw)
		SetActorRotation(cameraRotation, ETeleportType::TeleportPhysics);

		//setting Scale
		if (itemScaleCurve)
		{
			//Get curve value corresponding to the time passed
			const float scaleCurveValueOnGraph = itemScaleCurve->GetFloatValue(elapsedTime);

			SetActorScale3D(scaleCurveValueOnGraph*FVector(1.f));
		}
	}
}


void AItem::EnableCustomDepth()
{
	GetItemMesh()->SetRenderCustomDepth(true);
}

void AItem::DisableCustomDepth()
{
	GetItemMesh()->SetRenderCustomDepth(false);
}