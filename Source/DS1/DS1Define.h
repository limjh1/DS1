// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define COLLISION_OBJECT_INTERACTION ECC_GameTraceChannel1 // 프로젝트세팅-콜리전-오브젝트채널, DefaultEngine.ini에서 확인할 수 있음.
#define COLLISION_OBJECT_TARGETING   ECC_GameTraceChannel2

UENUM(BlueprintType)
enum class EDS1AttributeType : uint8
{
	Stamina,
	Health,
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right,
};

UENUM(BlueprintType)
enum class ESwitchingDirection : uint8
{
	None,
	Left,
	Right,
};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
	None,
	SwordShield,
	TwoHanded,
	MeleeFists
};

UENUM(BlueprintType)
enum class EWeaponCollisionType : uint8
{
	MainCollision,
	SecondCollision
};