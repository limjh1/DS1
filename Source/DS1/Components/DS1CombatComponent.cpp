// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1CombatComponent.h"
#include "Character/DS1Character.h"
#include "Items/DS1PickupItem.h"
#include "Equipment/DS1Weapon.h"

// Sets default values for this component's properties
UDS1CombatComponent::UDS1CombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDS1CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDS1CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDS1CombatComponent::SetWeapon(ADS1Weapon* NewWeapon)
{
	if (::IsValid(MainWeapon))
	{
		if (ADS1Character* OwnerCharacter = Cast<ADS1Character>(GetOwner()))
		{
			ADS1PickupItem* PickupItem = GetWorld()->SpawnActorDeferred<ADS1PickupItem>(
				ADS1PickupItem::StaticClass(), 
				OwnerCharacter->GetActorTransform(), 
				nullptr, 
				nullptr, 
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn // Physics처리
				);
			PickupItem->SetEquipmentClass(MainWeapon->GetClass());
			PickupItem->FinishSpawning(GetOwner()->GetActorTransform());

			MainWeapon->Destroy();
		}
	}

	MainWeapon = NewWeapon;
}

void UDS1CombatComponent::SetCombatEnabled(const bool bEnabled)
{
	bCombatEnabled = bEnabled;
	if (OnChangedCombat.IsBound())
	{
		OnChangedCombat.Broadcast(bCombatEnabled);
	}
}

