// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/DS1Weapon.h"
#include "DS1GameplayTags.h"
#include "Components/DS1CombatComponent.h"
#include "Data/DS1MontageActionData.h"
#include "Components/DS1WeaponCollisionComponent.h"
#include "Animation/DS1AnimInstance.h"
#include "Character/DS1Character.h"
#include "Kismet/GameplayStatics.h"

ADS1Weapon::ADS1Weapon()
{
	WeaponCollision = CreateDefaultSubobject<UDS1WeaponCollisionComponent>("WeaponCollision");
	WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);
	
	SecondWeaponCollision = CreateDefaultSubobject<UDS1WeaponCollisionComponent>("SecondWeaponCollision");
	SecondWeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Light, 7.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Running, 12.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Special, 15.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Heavy, 20.f);

	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Special, 2.1f);
}

void ADS1Weapon::EquipItem()
{
	Super::EquipItem();

	CombatComponent = GetOwner()->GetComponentByClass<UDS1CombatComponent>();
	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);

		const FName AttackSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;

		AttachToOwner(AttackSocket);

		WeaponCollision->SetWeaponMesh(Mesh); 

		// 장착 무기 CombatType으로 업데이트
		if (ADS1Character* OwnerCharacter = Cast<ADS1Character>(GetOwner()))
		{
			if (UDS1AnimInstance* Anim = Cast<UDS1AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}
		}

		WeaponCollision->AddIgnoredActor(GetOwner()); // 무기 소유자
	}
}

void ADS1Weapon::UnEquipItem()
{
	Super::UnEquipItem();
}

UAnimMontage* ADS1Weapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

float ADS1Weapon::GetStaminaCost(const FGameplayTag& InTag) const
{
	if (StaminaCostMap.Contains(InTag))
	{
		return StaminaCostMap[InTag];
	}

	return 0.0f;
}

float ADS1Weapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType();
		if (DamageMultiplierMap.Contains(LastAttackType))
		{
			const float Multiplier = DamageMultiplierMap[LastAttackType];
			return (BaseDamage * Multiplier);
		}
	}

	return BaseDamage;
}

void ADS1Weapon::ActivateCollision(EWeaponCollisionType InCollisionType)
{
	switch (InCollisionType)
	{
	case EWeaponCollisionType::MainCollision:
		WeaponCollision->TurnOnCollision();
		break;
	case EWeaponCollisionType::SecondCollision:
		SecondWeaponCollision->TurnOnCollision();
		break;
	default:
		break;
	}
}

void ADS1Weapon::DeactivateCollision(EWeaponCollisionType InCollisionType)
{
	switch (InCollisionType)
	{
	case EWeaponCollisionType::MainCollision:
		WeaponCollision->TurnOffCollision();
		break;
	case EWeaponCollisionType::SecondCollision:
		SecondWeaponCollision->TurnOffCollision();
		break;
	default:
		break;
	}
}

void ADS1Weapon::OnHitActor(const FHitResult& Hit)
{
	AActor* TargetActor = Hit.GetActor();
	FVector DamageDirection = GetOwner()->GetActorForwardVector();
	float AttackDamage = GetAttackDamage();

	// TargetActor의 TakeDamage로 수신
	UGameplayStatics::ApplyPointDamage(
		TargetActor,
		AttackDamage,
		DamageDirection,
		Hit,
		GetOwner()->GetInstigatorController(),
		this,
		nullptr
	);
}
