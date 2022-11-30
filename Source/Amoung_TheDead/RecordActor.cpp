// Fill out your copyright notice in the Description page of Project Settings.


#include "RecordActor.h"

// Sets default values
ARecordActor::ARecordActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Creating Root */
	scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(scene);

	//Creting mesh
	itemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	itemMesh->SetupAttachment(scene);

}

// Called when the game starts or when spawned
void ARecordActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARecordActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

