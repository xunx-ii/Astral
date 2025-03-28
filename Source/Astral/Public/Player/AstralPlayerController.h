// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AstralPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ASTRAL_API AAstralPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAstralPlayerController();

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
};
