// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_DS1AttackFinished.h"
#include "Character/DS1Character.h"


UAnimNotify_DS1AttackFinished::UAnimNotify_DS1AttackFinished(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotify_DS1AttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ADS1Character* Character = Cast<ADS1Character>(MeshComp->GetOwner()))
	{
		Character->AttackFinished(ComboResetDelay);
	}
}
