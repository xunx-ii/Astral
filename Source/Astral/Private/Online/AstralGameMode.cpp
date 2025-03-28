// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/AstralGameMode.h"
#include "System/AstralWorldSettings.h"
#include "System/AstralAssetManager.h"
#include "Online/AstralGameState.h"
#include "System/AstralPawnData.h"
#include "Player/AstralPlayerState.h"
#include "System/AstralExperienceDefinition.h"
#include "Player/AstralPlayerController.h"
#include "Player/AstralCharacterBase.h"

AAstralGameMode::AAstralGameMode()
{
	GameStateClass = AAstralGameState::StaticClass();
	PlayerControllerClass = AAstralPlayerController::StaticClass();
	PlayerStateClass = AAstralPlayerState::StaticClass();
	DefaultPawnClass = AAstralCharacterBase::StaticClass();

}

void AAstralGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}


UClass* AAstralGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UAstralPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

const UAstralPawnData* AAstralGameMode::GetPawnDataForController(const AController* InController) const
{
	if (InController != nullptr)
	{
		if (const AAstralPlayerState* AstralPlayerState = InController->GetPlayerState<AAstralPlayerState>())
		{
			if (const UAstralPawnData* PawnData = AstralPlayerState->GetPawnData<UAstralPawnData>())
			{
				return PawnData;
			}
		}
	}

	AAstralGameState* AstralGameState = Cast<AAstralGameState>(GameState);

	if (AstralGameState && AstralGameState->IsExperienceLoaded())
	{
		const UAstralExperienceDefinition* Experience = AstralGameState->GetCurrentExperience();

		if (Experience && Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		return UAstralAssetManager::Get().GetDefaultPawnData();
	}

	return nullptr;
}

void AAstralGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	UWorld* World = GetWorld();

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (AAstralWorldSettings* TypedWorldSettings = Cast<AAstralWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	UAstralAssetManager& AssetManager = UAstralAssetManager::Get();

	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogClass, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void AAstralGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogClass, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);
		AAstralGameState* AstralGameState = Cast<AAstralGameState>(GameState);
		if (AstralGameState)
		{
			AstralGameState->SetCurrentExperience(ExperienceId);
		}
	}
	else
	{
		UE_LOG(LogClass, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}
