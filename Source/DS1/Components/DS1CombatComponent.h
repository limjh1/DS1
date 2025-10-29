// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DS1CombatComponent.generated.h"

class ADS1Weapon;

DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnChangedCombat, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS1_API UDS1CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 컴뱃 상태 전환을 알리는 delegate
	FDelegateOnChangedCombat OnChangedCombat;

protected:
	UPROPERTY()
	ADS1Weapon* MainWeapon;

	UPROPERTY(EditAnywhere)
	bool bCombatEnabled = false;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag LastAttackType;

public:	
	// Sets default values for this component's properties
	UDS1CombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetWeapon(ADS1Weapon* NewWeapon);
	FORCEINLINE ADS1Weapon* GetMainWeapon() const { return MainWeapon; }

public:
	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	void SetCombatEnabled(const bool bEnabled);
	
	FORCEINLINE FGameplayTag GetLastAttackType() const { return LastAttackType; }
	FORCEINLINE void SetLastAttackType(const FGameplayTag& NewAttackTypeTag) { LastAttackType = NewAttackTypeTag; }
};
