// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AstralPlayerState.generated.h"

class UAstralPawnData;
class UAstralExperienceDefinition;
class UAstralAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAstralPawnDataLoaded, const UAstralPawnData* /*PawnData*/);

UCLASS()
class ASTRAL_API AAstralPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAstralPlayerState();

	virtual void PostInitializeComponents() override;
	
	template <class T>
	const T* GetPawnData() const { return Cast<T>(GamePawnData); }

	void SetPawnData(const UAstralPawnData* PawnData);

	UFUNCTION(BlueprintCallable, Category = "Astral|PlayerState")
	UAstralAbilitySystemComponent* GetAstralAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void CallOrRegister_OnPawnDataLoaded(FOnAstralPawnDataLoaded::FDelegate&& Delegate);
private:
	void OnExperienceLoaded(const UAstralExperienceDefinition* CurrentExperience);
	
protected:
	UFUNCTION()
	void OnRep_GamePawnData();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_GamePawnData)
	TObjectPtr<const UAstralPawnData> GamePawnData;

	FOnAstralPawnDataLoaded OnPawnDataLoaded;
private:
	bool bPawnDataIsLoaded;
	
	UPROPERTY(VisibleAnywhere, Category = "Lyra|PlayerState")
	TObjectPtr<UAstralAbilitySystemComponent> AbilitySystemComponent;
};
