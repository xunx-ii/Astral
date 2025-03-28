// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AstralPlayerController.h"
#include "Player/AstralPlayerState.h"
#include "Abilities/AstralAbilitySystemComponent.h"

AAstralPlayerController::AAstralPlayerController()
{

}

void AAstralPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{

	AAstralPlayerState* AstralPlayerState = CastChecked<AAstralPlayerState>(PlayerState, ECastCheckedType::NullAllowed);

	if (AstralPlayerState)
	{
		UAstralAbilitySystemComponent* AbilitySystemComponent = AstralPlayerState->GetAstralAbilitySystemComponent();
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
