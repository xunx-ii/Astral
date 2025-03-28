// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AstralPawnData.generated.h"

class UAstralAbilitySet;
class UAstralInputConfig;

UCLASS()
class ASTRAL_API UAstralPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Astral|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Astral|Abilities")
	TArray<TObjectPtr<UAstralAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Astral|Input")
	TObjectPtr<UAstralInputConfig> InputConfig;
};
