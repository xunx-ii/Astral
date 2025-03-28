// Fill out your copyright notice in the Description page of Project Settings.

#include "System/AstralAssetManager.h"
#include "System/AstralPawnData.h"

UAstralAssetManager::UAstralAssetManager()
{
	DefaultPawnData = nullptr;
}

UAstralAssetManager& UAstralAssetManager::Get()
{
	check(GEngine);

	if (UAstralAssetManager* Singleton = Cast<UAstralAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogClass, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to LyraAssetManager!"));

	return *NewObject<UAstralAssetManager>();
}

UObject* UAstralAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

const UAstralPawnData* UAstralAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}
