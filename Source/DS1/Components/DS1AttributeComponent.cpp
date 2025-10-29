// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1AttributeComponent.h"
#include "DS1GameplayTags.h"
#include "DS1StateComponent.h"

UDS1AttributeComponent::UDS1AttributeComponent()
{


}

void UDS1AttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UDS1AttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UDS1AttributeComponent::CheckHasEnoughStamina(float StaminaCost) const
{
	return (StaminaCost <= BaseStamina);
}

void UDS1AttributeComponent::DecreaseStamina(float StaminaCost)
{
	BaseStamina = FMath::Clamp(BaseStamina - StaminaCost, 0.f, MaxStamina);

	BroadcastAttributeChanged(EDS1AttributeType::Stamina);
}

void UDS1AttributeComponent::ToggleStaminaRegen(bool bEnabled, float StartDelay)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (bEnabled)
	{
		if (false == TimerManager.IsTimerActive(StaminaRegenTimerHandle))
		{
			TimerManager.SetTimer(StaminaRegenTimerHandle, this, &ThisClass::RegenStaminaHandler, 0.01f, true, StartDelay);
		}
	}
	else
	{
		TimerManager.ClearTimer(StaminaRegenTimerHandle);
	}
}

void UDS1AttributeComponent::BroadcastAttributeChanged(EDS1AttributeType InType) const
{
	if (OnAttributeChanged.IsBound())
	{
		float Ratio = 0.f;
		switch (InType)
		{
		case EDS1AttributeType::Stamina:
			Ratio = GetStaminaRatio();
			break;
		case EDS1AttributeType::Health:
			break;
		default:
			break;
		}

		OnAttributeChanged.Broadcast(InType, Ratio);
	}
}

void UDS1AttributeComponent::TakeDamageAmount(float DamageAmount)
{
	BaseHealth = FMath::Clamp(BaseHealth - DamageAmount, 0.f, MaxHealth);

	BroadcastAttributeChanged(EDS1AttributeType::Health);

	if (0.f >= BaseHealth)
	{
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}

		if (UDS1StateComponent* StateComp = GetOwner()->FindComponentByClass<UDS1StateComponent>())
		{
			StateComp->SetState(DS1GameplayTags::Character_State_Death);
		}
	}
}

void UDS1AttributeComponent::RegenStaminaHandler()
{
	BaseStamina = FMath::Clamp(BaseStamina + StaminaRegenRate, 0.f, MaxStamina);

	BroadcastAttributeChanged(EDS1AttributeType::Stamina);

	if (MaxStamina <= BaseStamina)
	{
		ToggleStaminaRegen(false);
	}
}

