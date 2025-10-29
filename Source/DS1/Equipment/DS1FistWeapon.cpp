// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/DS1FistWeapon.h"

#include "Animation/DS1AnimInstance.h"
#include "Character/DS1Character.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1WeaponCollisionComponent.h"

ADS1FistWeapon::ADS1FistWeapon()
{
}

void ADS1FistWeapon::EquipItem()
{
	CombatComponent = GetOwner()->GetComponentByClass<UDS1CombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);

		if (ADS1Character* OwnerCharacter = Cast<ADS1Character>(GetOwner()))
		{
			// 주먹을 무기로, 캐릭터의 스켈레탈 메시로 설정.
			WeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
			SecondWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());

			// 주먹이 무기이기 때문에 여기서 세팅. 실제 장착되는 무기들은 애니메이션의 Notify로 처리
			CombatComponent->SetCombatEnabled(true);

			// 장착 무기로 업뎃
			if (UDS1AnimInstance* Anim = Cast<UDS1AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}

			// 무기 소유한 Owner는 충돌 무시
			WeaponCollision->AddIgnoredActor(OwnerCharacter);
			SecondWeaponCollision->AddIgnoredActor(OwnerCharacter);
		}
	}
}
