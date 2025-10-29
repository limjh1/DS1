// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DS1Define.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DS1TargetingComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS1_API UDS1TargetingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetingRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FaceLockOnRotationSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

protected:
	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	UCameraComponent* Camera;

	UPROPERTY()
	AActor* LockedTargetActor;

	bool bIsLockOn = false;

public:	
	UDS1TargetingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void ToggleLockOn();
	void SwitchingLockedOnActor(ESwitchingDirection InDir);
	FORCEINLINE bool IsLockOn() const { return bIsLockOn; }

protected:
	// 후보군 검색
	void FindTargets(OUT TArray<AActor*>& OutTargetingActors) const;
	// 최종 액터, #TODO 그림 설명 필요 (정리 필요)
	AActor* FindClosestTarget(TArray<AActor*>& InTargets, ESwitchingDirection InDir = ESwitchingDirection::None) const;
	
	// 캐릭터가 카메라 회전에 동기화 되도록 설정 (회전, 이동방향, 락온 카메라 제어)
	void OrientCamera() const;
	void OrientMovement() const;
	void FaceLockOnActor() const;

	void LockOnTarget();
	void StopLockOn();

		
};
