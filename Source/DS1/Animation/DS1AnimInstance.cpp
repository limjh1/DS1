// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DS1AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/DS1Character.h"
#include "Components/DS1StateComponent.h"
#include "Components/DS1CombatComponent.h"
#include "KismetAnimationLibrary.h"

UDS1AnimInstance::UDS1AnimInstance()
{
}

void UDS1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(GetOwningActor());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
		if (UDS1CombatComponent* CombatComponent = Character->GetComponentByClass<UDS1CombatComponent>())
		{
			CombatComponent->OnChangedCombat.AddUObject(this, &ThisClass::OnChangedCombat);
		}
	}
}

void UDS1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == Character || nullptr == MovementComponent)
	{
		return;
	}

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D(); // Up 벡터는 필요 X

	bShouldMove = (3.f < GroundSpeed && FVector::ZeroVector != MovementComponent->GetCurrentAcceleration());
	bIsFalling = MovementComponent->IsFalling();	

	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
}

void UDS1AnimInstance::AnimNotify_ResetMovementInput()
{
	if (const ADS1Character* LocalCharacter = Cast<ADS1Character>(GetOwningActor()))
	{
		if (UDS1StateComponent* State = LocalCharacter->GetStateComponent())
		{
			State->ToggleMovementInput(true);
		}
	}
}

void UDS1AnimInstance::AnimNotify_ResetState()
{
	if (const ADS1Character* LocalCharacter = Cast<ADS1Character>(GetOwningActor()))
	{
		if (UDS1StateComponent* State = LocalCharacter->GetStateComponent())
		{
			State->ClearState();
		}
	}
}

void UDS1AnimInstance::UpdateCombatMode(const ECombatType InCombatType)
{
	CombatType = InCombatType;
}

void UDS1AnimInstance::OnChangedCombat(const bool bInCombatEnabled)
{
	bCombatEnabled = bInCombatEnabled;
}
