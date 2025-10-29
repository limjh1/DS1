// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DS1Targeting.generated.h"

// 다른 객체에서도 타겟팅 가능하게, 인터페이스 설정.

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDS1Targeting : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DS1_API IDS1Targeting
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnTargeted(bool bTargeted) = 0;
	virtual bool CanBeTargeted() = 0;
};
