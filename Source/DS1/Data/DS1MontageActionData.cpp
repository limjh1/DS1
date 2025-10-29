// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DS1MontageActionData.h"

UAnimMontage* UDS1MontageActionData::GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index)
{
	if (MontageGroupMap.Contains(GroupTag))
	{
		const FDS1MontageGroup& MontageGroup = MontageGroupMap[GroupTag];
		if (0 < MontageGroup.Animations.Num() && Index < MontageGroup.Animations.Num())
		{
			return MontageGroup.Animations[Index];
		}
	}

	return nullptr;
}
