// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AstralCharacterBase.h"
#include "Abilities/AstralAbilitySystemComponent.h"
#include "Player/AstralPlayerState.h"
#include "Player/AstralPlayerController.h"
#include "Player/AstralEnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "System/AstralPawnData.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "AstralGameplayTags.h"


AAstralCharacterBase::AAstralCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	

	AbilitySystemComponent = nullptr;
}


UAbilitySystemComponent* AAstralCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AAstralCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAstralPlayerState* AstralPlayerState = GetPlayerState<AAstralPlayerState>();
	if (AstralPlayerState)
	{
		InitializeAbilitySystem(AstralPlayerState->GetAstralAbilitySystemComponent(), this);
	}
}


void AAstralCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AAstralPlayerState* AstralPlayerState = GetPlayerState<AAstralPlayerState>();
	if (AstralPlayerState)
	{
		InitializeAbilitySystem(AstralPlayerState->GetAstralAbilitySystemComponent(), this);
	}
}


void AAstralCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAstralCharacterBase::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent->GetAvatarActor() == this)
	{
		AbilitySystemComponent->DestroyActiveState();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}
	}

	AbilitySystemComponent = nullptr;
}


void AAstralCharacterBase::InitializePlayerInput(class UInputComponent* PlayerInputComponent)
{
	if (AAstralPlayerController* AstralPlayerController = GetController<AAstralPlayerController>())
	{
		if (ULocalPlayer* LocalPlayer = AstralPlayerController->GetLocalPlayer())
		{
			UAstralEnhancedInputComponent* AstralEnhancedInputComponent = Cast<UAstralEnhancedInputComponent>(PlayerInputComponent);
			if (AstralEnhancedInputComponent)
			{
				UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
				Subsystem->ClearAllMappings();

				if (AAstralPlayerState* AstralPlayerState = GetPlayerState<AAstralPlayerState>())
				{
					const UAstralPawnData* AstralPawnData = AstralPlayerState->GetPawnData<UAstralPawnData>();
					if (AstralPawnData)
					{
						UAstralInputConfig* AstralInputConfig = AstralPawnData->InputConfig;
						if (AstralInputConfig)
						{
							for (const FInputMappingContextAndPriority& Mapping : AstralInputConfig->DefaultInputMappings)
							{
								if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
								{
									if (Mapping.bRegisterWithSettings)
									{
										if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
										{
											Settings->RegisterInputMappingContext(IMC);
										}

										FModifyContextOptions Options = {};
										Options.bIgnoreAllPressedKeysUntilRelease = false;					
										Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
									}
								}
							}

							if (AstralEnhancedInputComponent)
							{
								TArray<uint32> BindHandles;
								AstralEnhancedInputComponent->BindAbilityActions(AstralInputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

								AstralEnhancedInputComponent->BindNativeAction(AstralInputConfig, AstralGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
								AstralEnhancedInputComponent->BindNativeAction(AstralInputConfig, AstralGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
							}
						}
					}
				}
			}
		}
	}
}

void AAstralCharacterBase::InitializeAbilitySystem(UAstralAbilitySystemComponent* InAstralAbilitySystemComponent, AActor* InOwnerActor)
{
	if (AbilitySystemComponent == InAstralAbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	AActor* ExistingAvatar = InAstralAbilitySystemComponent->GetAvatarActor();

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != this))
	{
		UE_LOG(LogClass, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		ensure(!ExistingAvatar->HasAuthority());

		if (AAstralCharacterBase* OtherExistingAvatar = Cast<AAstralCharacterBase>(ExistingAvatar))
		{
			OtherExistingAvatar->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InAstralAbilitySystemComponent;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);
}

void AAstralCharacterBase::Input_Move(const FInputActionValue& InputActionValue)
{
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void AAstralCharacterBase::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		AddControllerYawInput(-Value.X);
	}

	if (Value.Y != 0.0f)
	{
		AddControllerPitchInput(Value.Y);
	}
}

void AAstralCharacterBase::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}


void AAstralCharacterBase::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

// Called every frame
void AAstralCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAstralCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AAstralPlayerState* AstralPlayerState = GetPlayerState<AAstralPlayerState>();
	if (AstralPlayerState)
	{
		AstralPlayerState->CallOrRegister_OnPawnDataLoaded(FOnAstralPawnDataLoaded::FDelegate::CreateLambda([this, PlayerInputComponent](const UAstralPawnData* PawnData)
			{
				this->InitializePlayerInput(PlayerInputComponent);
			}));
	}
	else 
	{
		UE_LOG(LogClass, Error, TEXT("AAstralCharacterBase::SetupPlayerInputComponent AAstralPlayerState is nullptr!!!!!"));
	}
}

