// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AstralGameState.generated.h"

class UAstralExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAstralExperienceLoaded, const UAstralExperienceDefinition* /*Experience*/);

UCLASS()
class ASTRAL_API AAstralGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AAstralGameState();

	bool IsExperienceLoaded() const;

	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	const UAstralExperienceDefinition* GetCurrentExperience() const;

	void CallOrRegister_OnExperienceLoaded(FOnAstralExperienceLoaded::FDelegate&& Delegate);
private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();

	void LoadingAssetsInTheExperience(const UAstralExperienceDefinition* Experience);

	void OnExperienceLoadComplete();
private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UAstralExperienceDefinition> CurrentExperience;

	FOnAstralExperienceLoaded OnExperienceLoaded;

	bool bIsExperienceLoaded;
};
