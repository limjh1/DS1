// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DS1WeaponCollisionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitActor, const FHitResult&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), meta=(DisplayName = "Weapon Collision") )
class DS1_API UDS1WeaponCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnHitActor OnHitActor;

protected:
	UPROPERTY(EditAnywhere)
	FName TraceStartSocketName;

	UPROPERTY(EditAnywhere)
	FName TraceEndSocketName;

protected:
	// Sphere 크기 (반지름)
	UPROPERTY(EditAnywhere)
	float TraceRadius = 15.f;  // 무기별로 변경되게 처리해야함.

	// Trace 대상 타입
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes; 
	
	// 충돌 처리 제외
	UPROPERTY(EditAnywhere)
	TArray<AActor*> IgnoredActors; 

	// 디버그 용
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

protected:
	// 무기 Mesh 
	UPROPERTY()
	UPrimitiveComponent* WeaponMesh; // Skeletal, Static 상위로

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	bool bIsCollisionEnabled = false;

public:	
	// Sets default values for this component's properties
	UDS1WeaponCollisionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void TurnOnCollision();
	void TurnOffCollision();

	void SetWeaponMesh(UPrimitiveComponent* MeshComp);
	
	void AddIgnoredActor(AActor* Actor);
	void RemoveIngoredActor(AActor* Actor);

protected:
	bool CanHitActor(AActor* Actor) const;
	void CollisionTrace();

};
