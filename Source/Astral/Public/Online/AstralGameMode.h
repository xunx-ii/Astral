// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AstralGameMode.generated.h"

class UAstralPawnData;

UCLASS()
class ASTRAL_API AAstralGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAstralGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;


	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UAstralPawnData* GetPawnDataForController(const AController* InController) const;
protected:
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
};
