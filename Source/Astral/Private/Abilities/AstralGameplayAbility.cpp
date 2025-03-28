// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AstralGameplayAbility.h"

UAstralGameplayAbility::UAstralGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	ActivationPolicy = EAstralAbilityActivationPolicy::OnInputTriggered;
}
