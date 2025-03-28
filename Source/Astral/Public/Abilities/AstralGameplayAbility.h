// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AstralGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EAstralAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,
};

UCLASS()
class ASTRAL_API UAstralGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UAstralGameplayAbility();

	EAstralAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Ability Activation")
	EAstralAbilityActivationPolicy ActivationPolicy;
};
