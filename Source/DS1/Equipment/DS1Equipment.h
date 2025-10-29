// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DS1Equipment.generated.h"

UCLASS()
class DS1_API ADS1Equipment : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Equipment | Mesh")
	UStaticMesh* MeshAsset;

	UPROPERTY(VisibleAnywhere, Category = "Equipment | Mesh")
	UStaticMeshComponent* Mesh;
	
public:	
	ADS1Equipment();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void EquipItem() {}
	virtual void UnEquipItem() {}
	virtual void AttachToOwner(FName SocketName);

};
