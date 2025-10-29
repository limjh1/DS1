// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DS1Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/DS1AttributeComponent.h"
#include "Components/DS1StateComponent.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1TargetingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/DS1PlayerHUDWidget.h"
#include "DS1GameplayTags.h"
#include "Equipment/DS1FistWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/DS1Interact.h"
#include "Equipment/DS1Weapon.h"

ADS1Character::ADS1Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// 회전값 적용 X
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 카메라 방향으로 캐릭터 회전
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.f, 500.f, 0.f);

		// 이동, 감속 속도
		Movement->MaxWalkSpeed = 500.f;
		Movement->BrakingDecelerationWalking = 2000.f;
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true; // 스프링암이 컨트롤러의 회전 값을 따라감

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	AttributeComponent = CreateDefaultSubobject<UDS1AttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UDS1StateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UDS1CombatComponent>(TEXT("Combat"));
	TargetingComponent = CreateDefaultSubobject<UDS1TargetingComponent>(TEXT("Targeting"));
}

void ADS1Character::BeginPlay()
{
	Super::BeginPlay();
	
	// Pleyer HUD
	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UDS1PlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
	
	// 주먹 무기
	if (FistWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		ADS1FistWeapon* FistWeapon = GetWorld()->SpawnActor<ADS1FistWeapon>(FistWeaponClass, GetActorTransform(), SpawnParams);
		FistWeapon->EquipItem();
	}
}

void ADS1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Stamina : %f"), AttributeComponent->GetBaseStamina()));
	GEngine->AddOnScreenDebugMessage(2, 1.5f, FColor::Cyan, FString::Printf(TEXT("MaxWalkSpeed: %f"), GetCharacterMovement()->MaxWalkSpeed));

}

void ADS1Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ADS1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Triggered, this, &ThisClass::Sprinting);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Canceled, this, &ThisClass::Rolling);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);

		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &ThisClass::ToggleCombat);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ThisClass::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::SpecialAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttack);

		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &ThisClass::LockOnTarget);
		EnhancedInputComponent->BindAction(LeftTargetAction, ETriggerEvent::Started, this, &ThisClass::LeftTarget);
		EnhancedInputComponent->BindAction(RightTargetAction, ETriggerEvent::Started, this, &ThisClass::RightTarget);
	}
}

bool ADS1Character::IsMoving() const
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		return (3.f < MoveComp->Velocity.Size2D() && FVector::Zero() != MoveComp->GetCurrentAcceleration());
	}

	return false;
}

bool ADS1Character::CanToggleCombat() const
{
	check(StateComponent);

	if (false == IsValid(CombatComponent->GetMainWeapon()))
	{
		return false;
	}

	// 주먹은 토글 X
	if (ECombatType::MeleeFists == CombatComponent->GetMainWeapon()->GetCombatType())
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Attacking);
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction); // 토글 중

	return (false == StateComponent->IsCurrentStateEqualToAny(CheckTags));
}

void ADS1Character::Move(const FInputActionValue& Values)
{
	check(StateComponent);

	if (false == StateComponent->MovementInputEnabled())
	{
		return;
	}

	FVector2D MovementVector = Values.Get<FVector2D>();
	if (nullptr != Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotator(0, Rotation.Yaw, 0);

		const FVector ForwardVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);

		// 주어진 월드 방향 벡터(보통 정규화 됨)를 따라 ScaleValue만큼 스케일된 이동 입력 추가.
		// ScaleValue가 0보다 작으면, 반대 방향
		// ScaleValue 아날로그로도 가능. 콘솔. 0~1까지의 값
		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);
	}
}

void ADS1Character::Look(const FInputActionValue& Values)
{
	// 락온 상태에서는 제어
	if (TargetingComponent && TargetingComponent->IsLockOn())
	{
		return;
	}

	FVector2D LookDirection = Values.Get<FVector2D>();

	if (nullptr != Controller)
	{
		AddControllerYawInput(LookDirection.X); // 좌우회전
		AddControllerPitchInput(LookDirection.Y); // 상하회전
	}
}

void ADS1Character::Sprinting()
{
	if (AttributeComponent->CheckHasEnoughStamina(5.f) && IsMoving()) // 최소 조건
	{
		AttributeComponent->ToggleStaminaRegen(false);

		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		AttributeComponent->DecreaseStamina(0.1f); // 감소

		bSprinting = true;
	}
	else
	{
		StopSprint();
	}
}

void ADS1Character::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	AttributeComponent->ToggleStaminaRegen(true);
	bSprinting = false;
}

void ADS1Character::Rolling()
{
	check(AttributeComponent);
	check(StateComponent);

	if (AttributeComponent->CheckHasEnoughStamina(15.f/*#TODO*/))
	{
		AttributeComponent->ToggleStaminaRegen(false);
		StateComponent->ToggleMovementInput(false);

		AttributeComponent->DecreaseStamina(15.f);

		PlayAnimMontage(RollingMontage);

		StateComponent->SetState(DS1GameplayTags::Character_State_Rolling);

		// 재충전 시작
		AttributeComponent->ToggleStaminaRegen(true, 1.5f);
	}
}

void ADS1Character::Interact()
{
	FHitResult OutHit;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	constexpr float Radius = 150.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));

	TArray<AActor*> ActorsToIgnore;
	if (AActor* MainWeapon = Cast<AActor>(CombatComponent->GetMainWeapon()))
	{
		ActorsToIgnore.Add(MainWeapon);
	}

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, // 
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true // 
	);

	if (bHit)
	{
		if (AActor* HitActor = OutHit.GetActor())
		{
			if (IDS1Interact* Interaction = Cast<IDS1Interact>(HitActor))
			{
				Interaction->Interact(this);
			}
		}
	}
}

void ADS1Character::ToggleCombat()
{
	check(CombatComponent);
	check(StateComponent);

	if (CombatComponent)
	{
		if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon())
		{
			if (CanToggleCombat())
			{
				StateComponent->SetState(DS1GameplayTags::Character_State_GeneralAction);

				if (CombatComponent->IsCombatEnabled())
				{
					PlayAnimMontage(Weapon->GetMontageForTag(DS1GameplayTags::Character_Action_Unequip));
				}
				else
				{
					PlayAnimMontage(Weapon->GetMontageForTag(DS1GameplayTags::Character_Action_Equip));
				}
			}
		}
	}
}

void ADS1Character::AutoToggleCombat()
{
	if (CombatComponent)
	{
		if (false == CombatComponent->IsCombatEnabled())
		{
			ToggleCombat();
		}
	}
}

void ADS1Character::Attack()
{
	const FGameplayTag AttackTypeTag = GetAttackPerform();
	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::SpecialAttack()
{
	const FGameplayTag AttackTypeTag = DS1GameplayTags::Character_Attack_Special;
	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::HeavyAttack()
{
	AutoToggleCombat();

	const FGameplayTag AttackTypeTag = DS1GameplayTags::Character_Attack_Heavy;
	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::LockOnTarget()
{
	check(TargetingComponent);

	TargetingComponent->ToggleLockOn();
}

void ADS1Character::LeftTarget()
{
	check(TargetingComponent);

	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Left);
}

void ADS1Character::RightTarget()
{
	check(TargetingComponent);

	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Right);
}

FGameplayTag ADS1Character::GetAttackPerform() const
{
	if (IsSprinting())
	{
		return DS1GameplayTags::Character_Attack_Running;
	}

	return DS1GameplayTags::Character_Attack_Light;
}

bool ADS1Character::CanPerformAttack(const FGameplayTag& AttackTypeTag) const
{
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);

	if (false == IsValid(CombatComponent->GetMainWeapon()))
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction);

	const float StaminaCost = CombatComponent->GetMainWeapon()->GetStaminaCost(AttackTypeTag);

	return (false == StateComponent->IsCurrentStateEqualToAny(CheckTags)
		&& CombatComponent->IsCombatEnabled()
		&& AttributeComponent->CheckHasEnoughStamina(StaminaCost));

	return false;
}

void ADS1Character::DoAttack(const FGameplayTag& AttackTypeTag)
{
	check(StateComponent);
	check(AttributeComponent);
	check(CombatComponent);

	if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(DS1GameplayTags::Character_State_Attacking);
		StateComponent->ToggleMovementInput(false);
		CombatComponent->SetLastAttackType(AttackTypeTag);

		AttributeComponent->ToggleStaminaRegen(false);

		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		if (nullptr == Montage)
		{
			// 콤보 한계
			ComboCounter = 0;
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		}

		PlayAnimMontage(Montage);

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegen(true, 1.5f);
	}
}

void ADS1Character::ExecuteComboAttack(const FGameplayTag& AttackTypeTag)
{
	if (StateComponent->GetCurrentState() != DS1GameplayTags::Character_State_Attacking)
	{
		if (bComboSequenceRunning && false == bCanComboInput)
		{
			// 애님은 끝났지만 아직 콤보 시퀀스가 유효할 때 - 추가 입력 기회 (#TODO 특정 효과?)
			++ComboCounter;
			UE_LOG(LogTemp, Warning, TEXT("Additional input : Combo Counter = %d"), ComboCounter);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT(">>> ComboeSequence Started <<<"));
			ResetCombo();
			bComboSequenceRunning = true;
		}

		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}
	else if (bCanComboInput)
	{
		// 콤보 윈도우가 열려 있을 때 - 최적의 타이밍 (#TODO 특정 효과?)
		bSavedComboInput = true;
	}
}

void ADS1Character::ResetCombo()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Reset"));

	bComboSequenceRunning = false;
	bCanComboInput = false;
	bSavedComboInput = false;
	ComboCounter = 0;
}

void ADS1Character::EnableComboWindow()
{
	bCanComboInput = true;
	UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

void ADS1Character::DisableComboWindow()
{
	check(CombatComponent);
	
	bCanComboInput = false;

	if (bSavedComboInput)
	{
		bSavedComboInput = false;
		++ComboCounter;
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: Advancing to next Combo = %d"), ComboCounter);
		DoAttack(CombatComponent->GetLastAttackType());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: No input recevied"));
	}
}

void ADS1Character::AttackFinished(const float ComboResetDelay)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Finished"));
	if (StateComponent)
	{
		StateComponent->ToggleMovementInput(true);
	}

	// ComboResetDelay 후 콤보 시퀀스 종료 (약간의 딜레이로 여유 시간 줘서 콤보 이어가게 추가 입력)
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
}

void ADS1Character::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

void ADS1Character::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}
