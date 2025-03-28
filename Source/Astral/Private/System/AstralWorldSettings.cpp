// Fill out your copyright notice in the Description page of Project Settings.


#include "System/AstralWorldSettings.h"
#include "System/AstralExperienceDefinition.h"
#include "Engine/AssetManager.h"


AAstralWorldSettings::AAstralWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FPrimaryAssetId AAstralWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

		if (!Result.IsValid())
		{
			UE_LOG(LogClass, Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
				*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}
