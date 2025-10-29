// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/DS1Targeting.h"
#include "DS1Enemy.generated.h"

class UWidgetComponent;
class USphereComponent;
class UDS1StateComponent;
class UDS1AttributeComponent;
class USoundCue;
class UParticleSystem;

UCLASS()
class DS1_API ADS1Enemy : public ACharacter, public IDS1Targeting
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TargetingSphereComponent;

	UPROPERTY(VisibleAnywhere)
	UDS1AttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	UDS1StateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* LockOnWidgetComponent;

protected:
	// Effect Section
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* ImpactParticle;

protected:
	// Montage Section
	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimFront;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimBack;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimLeft;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimRight;

public:
	ADS1Enemy();

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void OnDeath();

protected:
	void ImpactEffect(const FVector Location);
	void HitReaction(const AActor* Attacker);
	UAnimMontage* GetHitReactAnimation(const AActor* Attacker) const;

public:
	virtual void OnTargeted(bool bTargeted) override;
	virtual bool CanBeTargeted() override;

};
