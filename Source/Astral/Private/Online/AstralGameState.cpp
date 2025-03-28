// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/AstralGameState.h"
#include "Net/UnrealNetwork.h"
#include "System/AstralAssetManager.h"
#include "System/AstralExperienceDefinition.h"

AAstralGameState::AAstralGameState()
{
	CurrentExperience = nullptr;
	bIsExperienceLoaded = false;
}

bool AAstralGameState::IsExperienceLoaded() const
{
	return (bIsExperienceLoaded == true) && (CurrentExperience != nullptr);
}

void AAstralGameState::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UAstralAssetManager& AssetManager = UAstralAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
	TSubclassOf<UAstralExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass);
	const UAstralExperienceDefinition* Experience = GetDefault<UAstralExperienceDefinition>(AssetClass);

	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	CurrentExperience = Experience;
	StartExperienceLoad();
}

const UAstralExperienceDefinition* AAstralGameState::GetCurrentExperience() const
{
	return CurrentExperience;
}

void AAstralGameState::CallOrRegister_OnExperienceLoaded(FOnAstralExperienceLoaded::FDelegate&& Delegate)
{
	if (CurrentExperience != nullptr)
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void AAstralGameState::OnRep_CurrentExperience()
{
	StartExperienceLoad();
}

void AAstralGameState::StartExperienceLoad()
{
	// 开始加载资源
	LoadingAssetsInTheExperience(CurrentExperience);
}

void AAstralGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentExperience);
}

void AAstralGameState::LoadingAssetsInTheExperience(const UAstralExperienceDefinition* Experience)
{
	UAstralAssetManager& AssetManager = UAstralAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(Experience->GetPrimaryAssetId());

	// Load assets associated with the experience
	TArray<FName> BundlesToLoad;
	BundlesToLoad.Add(TEXT("Equipped"));

	const ENetMode OwnerNetMode = GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundlesToLoad.Add(TEXT("Client"));
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(TEXT("Server"));
	}

	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if (RawAssetList.Num() > 0)
	{
		RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartExperienceLoad()"));
	}

	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// Assets were already loaded, call the delegate now
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}
}

void AAstralGameState::OnExperienceLoadComplete()
{
	bIsExperienceLoaded = true;

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	// Apply any necessary scalability settings
}
