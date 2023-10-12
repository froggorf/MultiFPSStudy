// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BlasterCharacter.generated.h"

class UInputAction;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//[복제-2] 서버로부터 Replicated (복제) 가져오는 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//[컴퍼넌트] 컴퍼넌트가 생성될 때 실행되는 함수
	virtual void PostInitializeComponents() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere,Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* DefaultContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Jump;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_EquipWeapon;

	UPROPERTY(EditAnywhere, Category = "HUD", BlueprintReadOnly, meta = (AllowPrivateAccess = true)) //변수를 블루프린트에 노출
	class UWidgetComponent* OverheadWidget;

	//[복제-1]서버에서 복제로 생성되는 것에 대한 속성
	//UPROPERTY(Replicated)
	//[복제-4] 복제될 때 Rep_Notify 도 적용되도록
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;

	//인자를 안넘겨줄 수 있지만, 제거할 때 등에 대비해서
	// 같은 클래스의 것은 변수로 넘겨줄 수 있음
	UFUNCTION()
	//void OnRep_OverlappingWeapon();
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);


	//RPC - Remote Procedure Call (로컬에서 호출 되지만, 호출 머신이 아닌 다른 머신에서 원격 실행되는 함수)
	//일회성 행동들 - Reliable
	UFUNCTION(Server,Reliable)
	void Server_EquipButtonPressed();

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipButtonPressed(const FInputActionValue& Value);
	


public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
};
