// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1TargetingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/DS1Character.h"
#include "Interfaces/DS1Targeting.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UDS1TargetingComponent::UDS1TargetingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDS1TargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Camera = Character->GetComponentByClass<UCameraComponent>();
	}
	
}


// Called every frame
void UDS1TargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (false == bIsLockOn)
	{
		return;
	}

	if (false == IsValid(Character))
	{
		return;
	}

	if (false == IsValid(LockedTargetActor))
	{
		return;
	}

	const float Distance = FVector::Distance(Character->GetActorLocation(), LockedTargetActor->GetActorLocation());
	if (IDS1Targeting* Targeting = Cast<IDS1Targeting>(LockedTargetActor))
	{
		// 거리 멀어지면 중단
		if (false == Targeting->CanBeTargeted() || TargetingRadius < Distance)
		{
			StopLockOn();
		}
		else
		{
			FaceLockOnActor();
		}
	}
}

void UDS1TargetingComponent::ToggleLockOn()
{
	if (bIsLockOn)
	{
		StopLockOn();
	}
	else
	{
		LockOnTarget();
	}
}

void UDS1TargetingComponent::SwitchingLockedOnActor(ESwitchingDirection InDir)
{
	if (::IsValid(LockedTargetActor))
	{
		if (IDS1Targeting* Targeting = Cast<IDS1Targeting>(LockedTargetActor))
		{
			// 기존 타겟 해제
			Targeting->OnTargeted(false);

			// 신규
			TArray<AActor*> OutTargets;
			FindTargets(OutTargets);
			AActor* TargetActor = FindClosestTarget(OutTargets, InDir);

			if (::IsValid(TargetActor))
			{
				if (IDS1Targeting* NewTargeting = Cast<IDS1Targeting>(TargetActor))
				{
					LockedTargetActor = TargetActor;
					NewTargeting->OnTargeted(true);
				}
			}
			else
			{
				StopLockOn();
			}
		}
	}
}

void UDS1TargetingComponent::FindTargets(OUT TArray<AActor*>& OutTargetingActors) const
{
	check(Character);

	TArray<FHitResult> OutHits;
	const FVector Start = Character->GetActorLocation();
	const FVector End = Start;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_TARGETING));

	TArray<AActor*> ActorsToIgnore;

	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TargetingRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		DrawDebugType,
		OutHits,
		true
	);

	if (bHit)
	{
		for (const FHitResult& HitResult : OutHits)
		{
			AActor* HitActor = HitResult.GetActor();
			if (IDS1Targeting* Targeting = Cast<IDS1Targeting>(HitActor))
			{
				if (Targeting->CanBeTargeted())
				{
					OutTargetingActors.Add(HitActor);
				}
			}
		}
	}
}

AActor* UDS1TargetingComponent::FindClosestTarget(TArray<AActor*>& InTargets, ESwitchingDirection InDir) const
{
	float TargetCompareValue = 0.f;
	AActor* ClosestTarget = nullptr;

	for (const AActor* TargetActor : InTargets)
	{
		if (ESwitchingDirection::None != InDir && TargetActor == LockedTargetActor)
		{
			continue;
		}

		FHitResult OutHit;
		const FVector Start = Camera->GetComponentLocation();
		const FVector End = TargetActor->GetActorLocation();

		TArray<AActor*> ActorsToIgnore;

		// 대상이 ECC_Visibility 채널에 대해서 Response가 Block으로 설정되어있는지 확인 필요함.
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetOwner(),
			Start,
			End,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			DrawDebugType,
			OutHit,
			true
		);

		if (bHit)
		{
			// 카메라 시야 기준으로 내적 계사는 카메라 기준
			// Left 방향에 따른 내적 계산
			if (ESwitchingDirection::Left == InDir)
			{
				// 비교대상 vector가 Normal방향이기 때문에 반대로 계산됨.
				if (false == (0.f < FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal)))
				{
					continue;
				}
			}
			// Right
			if (ESwitchingDirection::Right == InDir)
			{
				if (false == (0.f > FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal)))
				{
					continue;
				}
			}

			AActor* HitActor = OutHit.GetActor();

			// 플레이어와 가장 가까운 타겟, 전방에 가장 가까운 타겟찾기위해, 카메라 Forward와 LookAt 내적
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), HitActor->GetActorLocation());
			float CheckValue = FVector::DotProduct(Camera->GetForwardVector(), LookAtRotation.Vector());

			if (TargetCompareValue < CheckValue)
			{
				TargetCompareValue = CheckValue;
				ClosestTarget = HitActor;
			}
		}
	}

	return ClosestTarget;
}

void UDS1TargetingComponent::OrientCamera() const
{
	check(Character);

	// 카메라 따라가기
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void UDS1TargetingComponent::OrientMovement() const
{
	check(Character);

	// 이동방향 따라가기
	Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void UDS1TargetingComponent::FaceLockOnActor() const
{
	check(Character);

	const FRotator CurrentControlRotation = Character->GetControlRotation();

	const FVector TargetLocation = LockedTargetActor->GetActorLocation() - FVector(0.f, 0.f, 150.f); // 카메라 위치 보정
	const FRotator TargetLookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), TargetLocation);

	FRotator InterpRotation = FMath::RInterpTo(CurrentControlRotation, TargetLookAtRotation, GetWorld()->GetDeltaSeconds(), FaceLockOnRotationSpeed);

	// Roll 은 기존 ControlRotation.Roll 값으로 유지하고 Pitch/Yaw값만 InterpRotation의 값으로 적용
	Character->GetController()->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw, CurrentControlRotation.Roll));
}

void UDS1TargetingComponent::LockOnTarget()
{
	TArray<AActor*> OutTargets;
	FindTargets(OutTargets);

	AActor* TargetActor = FindClosestTarget(OutTargets);

	if (::IsValid(TargetActor))
	{
		LockedTargetActor = TargetActor;
		bIsLockOn = true;
		if (IDS1Targeting* Targeting = Cast<IDS1Targeting>(LockedTargetActor))
		{
			Targeting->OnTargeted(bIsLockOn);
		}

		OrientCamera();
	}
}

void UDS1TargetingComponent::StopLockOn()
{
	bIsLockOn = false;
	if (IDS1Targeting* Targeting = Cast<IDS1Targeting>(LockedTargetActor))
	{
		Targeting->OnTargeted(bIsLockOn);
	}
	LockedTargetActor = nullptr;

	OrientMovement();
}

