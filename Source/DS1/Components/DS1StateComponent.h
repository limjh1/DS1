// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DS1StateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS1_API UDS1StateComponent : public UActorComponent
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State")
	bool bMovementInputEnabled = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State")
	FGameplayTag CurrentState;

public:	
	// Sets default values for this component's properties
	UDS1StateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE bool MovementInputEnabled() const { return bMovementInputEnabled; }

	void ToggleMovementInput(bool bEnabled, float Duration = 0.f);
	
protected:
	UFUNCTION() // 리플렉션
	void MovementInputEnableAction();

public:
	FORCEINLINE void SetState(const FGameplayTag NewState) { CurrentState = NewState; }
	FORCEINLINE FGameplayTag GetCurrentState() const { return CurrentState; }
	
	void ClearState();

	bool IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const;



};
