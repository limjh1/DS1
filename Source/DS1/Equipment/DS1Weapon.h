// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/DS1Equipment.h"
#include "GameplayTagContainer.h"
#include "DS1Define.h"
#include "DS1Weapon.generated.h"

class UDS1WeaponCollisionComponent;
class UDS1MontageActionData;
class UDS1CombatComponent;
/**
 * 
 */
UCLASS()
class DS1_API ADS1Weapon : public ADS1Equipment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName UnequipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	ECombatType CombatType = ECombatType::SwordShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	UDS1MontageActionData* MontageActionData;

protected:
	// Componente Section
	UPROPERTY(VisibleAnywhere)
	UDS1WeaponCollisionComponent* WeaponCollision; 

	UPROPERTY(VisibleAnywhere)
	UDS1WeaponCollisionComponent* SecondWeaponCollision; // 이도류 같은 쌍검, 앙쪽 손 쥘 때 필요. 주먹

protected:
	UPROPERTY()
	UDS1CombatComponent* CombatComponent; // 캐싱

protected:
	// Data Section
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f; // #TODO GAS 적용 필요. 무기의 기본 데미지

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap; // 공격 종류에 따른 데미지 승수

public:
	ADS1Weapon();

public:
	virtual void EquipItem() override;
	virtual void UnEquipItem() override;
	
	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;

	float GetStaminaCost(const FGameplayTag& InTag) const;
	float GetAttackDamage() const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; }
	FORCEINLINE UDS1WeaponCollisionComponent* GetCollision() const { return WeaponCollision; }
	FORCEINLINE ECombatType GetCombatType() const { return CombatType; }

public:
	virtual void ActivateCollision(EWeaponCollisionType InCollisionType);
	virtual void DeactivateCollision(EWeaponCollisionType InCollisionType);

public:
	// 무기의 Collision에 검출된 액터에 Damage 전달
	void OnHitActor(const FHitResult& Hit);
};
