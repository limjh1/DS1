// Fill out your copyright notice in the Description page of Project Settings.


#include "Kismet/KismetSystemLibrary.h"
#include "Components/DS1StateComponent.h"

// Sets default values for this component's properties
UDS1StateComponent::UDS1StateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDS1StateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDS1StateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDS1StateComponent::ToggleMovementInput(bool bEnabled, float Duration)
{
	if (bEnabled)
	{
		FLatentActionInfo LatenAction;
		LatenAction.CallbackTarget = this;
		LatenAction.ExecutionFunction = "MovementInputEnableAction"; // 리플렉션
		LatenAction.Linkage = 0;
		LatenAction.UUID = 0;

		// 현재 들어온 것을 기준으로 다시 처음부터 실행.
		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), Duration, LatenAction);
	}
	else
	{
		bMovementInputEnabled = false;
	}
}

void UDS1StateComponent::MovementInputEnableAction()
{
	bMovementInputEnabled = true;
	ClearState();
}

void UDS1StateComponent::ClearState()
{
	CurrentState = FGameplayTag::EmptyTag;
}

bool UDS1StateComponent::IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const
{
	// 아래와 같은 연산
	//if ("Attacking" == CurrentState || "Rolling" == CurrentState)
	//{
	//	return true;
	//}

	return TagsToCheck.HasTagExact(CurrentState);
}

