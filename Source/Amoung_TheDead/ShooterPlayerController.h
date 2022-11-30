// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AMOUNG_THEDEAD_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AShooterPlayerController();

protected:

	virtual void BeginPlay() override;

private:

	/* Ref to the overall widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Details", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> PlayerOverlayClass;

	/* variable to hold widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Details", meta = (AllowPrivateAccess = "true"))
	UUserWidget* widgetOverley;
};
