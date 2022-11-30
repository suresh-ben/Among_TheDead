// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"

AAmmo::AAmmo():
	ammoType(EAmmoType::EAM_9)
{
	//Constructing ammo
	ammoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ammo Mesh"));
	SetRootComponent(ammoMesh);
	
	ammoCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Ammo collision spher"));
	ammoCollisionSphere->SetupAttachment(RootComponent);
	ammoCollisionSphere->SetSphereRadius(75.f);

	//Changing attachments
	GetCollisionBox()->SetupAttachment(RootComponent);
	GetPickupWidget()->SetupAttachment(RootComponent);
	GetAreaSphere()->SetupAttachment(RootComponent);
	GetItemBase()->SetupAttachment(RootComponent);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	ammoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoSphereCollisionOverlap);
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmo::SetItemProperties(EItemState state)
{
	Super::SetItemProperties(state);

	switch (state)
	{
	case EItemState::EIS_Pickup:

		//Set mesh properties
		ammoMesh->SetSimulatePhysics(false);
		ammoMesh->SetEnableGravity(true);
		ammoMesh->SetVisibility(true);
		ammoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ammoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_EquipInterping:

		//Set mesh properties
		ammoMesh->SetSimulatePhysics(false);
		ammoMesh->SetEnableGravity(false);
		ammoMesh->SetVisibility(true);
		ammoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ammoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_PickedUp:

		break;

	case EItemState::EIS_Equipped:
		//Set pickupwidget of this weapon to hide
		GetPickupWidget()->SetVisibility(false);

		//Set mesh properties
		ammoMesh->SetSimulatePhysics(false);
		ammoMesh->SetEnableGravity(false);
		ammoMesh->SetVisibility(true);
		ammoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ammoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_Falling:

		//Set mesh properties
		ammoMesh->SetSimulatePhysics(true);
		ammoMesh->SetEnableGravity(true);
		ammoMesh->SetVisibility(true);
		ammoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ammoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ammoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		break;

	}

}

void AAmmo::AmmoSphereCollisionOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor && Cast<AShooterCharacter>(otherActor))
	{
		auto playerCharacter = Cast<AShooterCharacter>(otherActor);

		//Limit--Bullets-- ToDo
		StartItemCurve(playerCharacter);

	}
}
