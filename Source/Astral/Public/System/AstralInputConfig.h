// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AstralInputConfig.generated.h"

class UInputMappingContext;
class UInputAction;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FAstralInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


USTRUCT(BlueprintType)
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	UPROPERTY(EditAnywhere, Category="Input")
	int32 Priority = 0;
	
	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};

UCLASS()
class ASTRAL_API UAstralInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FAstralInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FAstralInputAction> AbilityInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Astral|Input")
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;
};
