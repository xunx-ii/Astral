// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "AstralWorldSettings.generated.h"

class UAstralExperienceDefinition;

UCLASS()
class ASTRAL_API AAstralWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:

	AAstralWorldSettings(const FObjectInitializer& ObjectInitializer);

	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UAstralExperienceDefinition> DefaultGameplayExperience;
	
};
