// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS1Define.h"
#include "Components/ActorComponent.h"
#include "DS1AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EDS1AttributeType, float);
DECLARE_MULTICAST_DELEGATE(FOnDeath);

UCLASS(classGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DS1_API UDS1AttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 스탯 변경
	FDelegateOnAttributeChanged OnAttributeChanged;

	// 죽음 알림
	FOnDeath OnDeath;

protected:
	// #TODO GAS
	UPROPERTY(EditAnywhere, Category="Stamina")
	float BaseStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenRate = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Health")
	float BaseHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 100.f;

	// 스테미나 재충전 타이머 핸들
	FTimerHandle StaminaRegenTimerHandle;
	
public:	
	UDS1AttributeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE float GetBaseStamina() const { return BaseStamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	FORCEINLINE float GetBaseHealth() const { return BaseHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	FORCEINLINE float GetStaminaRatio() const { return (BaseStamina / MaxStamina); }

	bool CheckHasEnoughStamina(float StaminaCost) const;
	void DecreaseStamina(float StaminaCost);

	void ToggleStaminaRegen(bool bEnabled, float StartDelay = 2.f);

	void BroadcastAttributeChanged(EDS1AttributeType InType) const;

	void TakeDamageAmount(float DamageAmount);

private:
	void RegenStaminaHandler();

};
