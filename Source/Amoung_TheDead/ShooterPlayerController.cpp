// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{

}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Check overlay calss
	if (PlayerOverlayClass)
	{
		widgetOverley = CreateWidget<UUserWidget>(this, PlayerOverlayClass);

		if (widgetOverley)
		{
			widgetOverley->AddToViewport();
			widgetOverley->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
