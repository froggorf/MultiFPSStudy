// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UWidgetComponent;

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
	void ShowPickupWidget(bool bShowWidget) { if (PickupWidget) PickupWidget->SetVisibility(bShowWidget); }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	UPROPERTY(VisibleAnywhere,Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	//z캐릭터와 겹치는 부분을 체크하는 컴퍼넌트,
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class USphereComponent* AreaSphere;

	UFUNCTION()
	void OnRep_WeaponState();
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UWidgetComponent* PickupWidget;
public:	
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }

};
