// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),	//바닥에 놓여진 상태
	EWS_Equipped UMETA(DisplayName = "Equipped"),	//장착된 상태
	EWS_Dropped UMETA(DisplayName = "Dropped"),	//무기를 드랍하는 상태

	EWS_MAX UMETA(DisplayName = "DefaultMAX"),	//EWeaponSTate의 수
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere,Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	//z캐릭터와 겹치는 부분을 체크하는 컴퍼넌트,
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class UWidgetComponent* PickupWidget;
public:	
	

};
