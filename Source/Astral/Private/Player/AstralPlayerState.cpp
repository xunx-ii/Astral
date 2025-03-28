// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AstralPlayerState.h"
#include "System/AstralExperienceDefinition.h"
#include "Online/AstralGameState.h"
#include "Online/AstralGameMode.h"
#include "Net/UnrealNetwork.h"
#include "System/AstralPawnData.h"
#include "System/AstralAbilitySet.h"
#include "Engine/World.h"
#include "Abilities/AstralAbilitySystemComponent.h"


AAstralPlayerState::AAstralPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAstralAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	SetNetUpdateFrequency(100.0f);

	bPawnDataIsLoaded = false;
}

void AAstralPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
	}
	
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AAstralGameState* AstralGameState = Cast<AAstralGameState>(GetWorld()->GetGameState());
		if (AstralGameState)
		{
			AstralGameState->CallOrRegister_OnExperienceLoaded(FOnAstralExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
		}
	}
}

void AAstralPlayerState::SetPawnData(const UAstralPawnData* PawnData)
{

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (GamePawnData)
	{
		UE_LOG(LogClass, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(PawnData), *GetNameSafe(this), *GetNameSafe(GamePawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GamePawnData, this);
	GamePawnData = PawnData;

	for (const UAstralAbilitySet* AbilitySet : GamePawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	bPawnDataIsLoaded = true;

	OnPawnDataLoaded.Broadcast(GamePawnData);
	OnPawnDataLoaded.Clear();

	ForceNetUpdate();
}

UAbilitySystemComponent* AAstralPlayerState::GetAbilitySystemComponent() const
{
	return GetAstralAbilitySystemComponent();
}


void AAstralPlayerState::CallOrRegister_OnPawnDataLoaded(FOnAstralPawnDataLoaded::FDelegate&& Delegate)
{
	if (bPawnDataIsLoaded)
	{
		Delegate.Execute(GamePawnData);
	}
	else
	{
		OnPawnDataLoaded.Add(MoveTemp(Delegate));
	}
}

void AAstralPlayerState::OnExperienceLoaded(const UAstralExperienceDefinition* CurrentExperience)
{
	if (AAstralGameMode* AstralGameMode = GetWorld()->GetAuthGameMode<AAstralGameMode>())
	{
		if (const UAstralPawnData* NewPawnData = AstralGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogClass, Error, TEXT("AAstralPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void AAstralPlayerState::OnRep_GamePawnData()
{
	bPawnDataIsLoaded = true;
}

void AAstralPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, GamePawnData, SharedParams);
}