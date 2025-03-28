// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AstralExperienceDefinition.generated.h"

class UAstralPawnData;

UCLASS(BlueprintType, Const)
class ASTRAL_API UAstralExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAstralExperienceDefinition();

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UAstralPawnData> DefaultPawnData;
	
};
