// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_DS1WpCollision.h"

#include "Interfaces/DS1CombatInterface.h"

UAnimNotifyState_DS1WpCollision::UAnimNotifyState_DS1WpCollision(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAnimNotifyState_DS1WpCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (IDS1CombatInterface* CombatInterface = Cast<IDS1CombatInterface>(OwnerActor))
		{
			CombatInterface->ActivateWeaponCollision(CollisionType);
		}

		// 이런거 수정에서 감동
		//if (const UDS1CombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UDS1CombatComponent>())
		//{
		//	const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon();
		//	if (::IsValid(Weapon))
		//	{
		//		Weapon->GetCollision()->TurnOnCollision();
		//	}
		//}
	}
}

void UAnimNotifyState_DS1WpCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (IDS1CombatInterface* CombatInterface = Cast<IDS1CombatInterface>(OwnerActor))
		{
			CombatInterface->DeactivateWeaponCollision(CollisionType);
		}
	}
}
