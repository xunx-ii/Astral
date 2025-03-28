// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AstralAssetManager.generated.h"

class UAstralPawnData;

UCLASS(Config = Game)
class ASTRAL_API UAstralAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	UAstralAssetManager();

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer);

	static UAstralAssetManager& Get();
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	const UAstralPawnData* GetDefaultPawnData() const;

protected:
	UPROPERTY(Config)
	TSoftObjectPtr<UAstralPawnData> DefaultPawnData;
};


template<typename AssetType>
AssetType* UAstralAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
	}

	return LoadedAsset;
}