// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Interfaces/DS1CombatInterface.h"
#include "DS1Character.generated.h"

class ADS1FistWeapon;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UDS1AttributeComponent;
class UDS1PlayerHUDWidget;
class UDS1StateComponent;
class UDS1CombatComponent;
class UDS1TargetingComponent;

UCLASS()
class DS1_API ADS1Character : public ACharacter, public IDS1CombatInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category= "Input")
	UInputAction* SprintRollingAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleCombatAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* HeavyAttackAction;

	// LockOn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LockOnTargetAction;

	// 왼쪽으로 타겟 전환
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftTargetAction;

	// 오른쪽으로 타겟 전환
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RightTargetAction;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess="true"))
	UDS1AttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1StateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1CombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1TargetingComponent* TargetingComponent;

protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	UDS1PlayerHUDWidget* PlayerHUDWidget;

protected:
	// 주먹  무기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADS1FistWeapon> FistWeaponClass;

protected:
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float SprintSpeed = 750.f;

	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float NormalSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, Category = "Sprinting")
	bool bSprinting = false;

protected:
	// Combo Section
	bool bComboSequenceRunning = false; // 콤보 시퀀스 진행중
	bool bCanComboInput = false;		// 입력 가능 여부
	int32 ComboCounter = 0;
	bool bSavedComboInput = false;		// 콤보 입력 여부
	FTimerHandle ComboResetTimerHandle; 

protected:
	// Montage Section
	UPROPERTY(EditAnywhere, Category = "Monatge")
	UAnimMontage* RollingMontage;

public:
	ADS1Character();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override; // 빙의, 컨트롤러 변경 시 호출

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	FORCEINLINE UDS1StateComponent* GetStateComponent() const { return StateComponent; }

protected: // INPUT 
	bool IsMoving() const;
	bool CanToggleCombat() const;
	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	void Move(const FInputActionValue& Values);
	void Look(const FInputActionValue& Values);

	void Sprinting();
	void StopSprint();

	void Rolling();

	void Interact();

	void ToggleCombat();
	void AutoToggleCombat(); // 무기 집기전 공격버튼 시 발동

	// Attack
	void Attack();
	void SpecialAttack();
	void HeavyAttack();

	// LockOn
	void LockOnTarget();
	void LeftTarget();
	void RightTarget();

protected:
	// 현 상태에서 수행 가능한 일반공격
	FGameplayTag GetAttackPerform() const;

	bool CanPerformAttack(const FGameplayTag& AttackTypeTag) const;
	void DoAttack(const FGameplayTag& AttackTypeTag);
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);
	void ResetCombo();

public:
	// Combo AnimNotify Section
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

public:
	void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
};
