// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "../Weapon/Weapon.h"
#include "Blaster/BlasterComponent/CombatComponent.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Widget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	//Combat Component를 Replicating Component로 만들기
	//Replicate 될 수 있게만 해주면 됨, GetLifetimeReplicatedProps라던가 기타 등등을 설정할 필요가 없다고함
	CombatComponent->SetIsReplicated(true);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//[복제-3] 복제는 변수가 변경될때만 작동
	//아래 매크로의 경우 모든 리모트프록시에게도 적용되어서 모든 클라이언트에게 되므로
	//특정 클라이언트에게만 되도록 하는 매크로를 사용해야함
	//DOREPLIFETIME(ABlasterCharacter, OverlappingWeapon);

	//조건으로 OwnerOnly로 지정하면, 가진 클라이언트 에게만 복사됨
	//->서버에서는 보이지만 클라이언트에선 당사자만 보임
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	

	//[복제 - 전체정리]
	// DOREPLIFETIME_CONDITION() 매크로를 통해 OverlappingWeapon이 변경되면 클라이언트에게 복제를 진행
	// AWeapon::OnSphereOverlap이 되면 BlasterCharacter->SetOverlappingWeapon(this) 가 서버에서만 작동
	// Tick에서 if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true); 로 확인하면 안좋음, 그래서 복제가 됐을 때 호출되는 함수인
	// "Rep_Notify"를 만듦

	// UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	// class AWeapon* OverlappingWeapon;
	// UFUNCTION()
	// void OnRep_OverlappingWeapon();

	// 이 이후엔 서버쪽에서도 보이지 않게 되는데,
	// 복제될 때만 호출되게 하여 UI를 보이는데,
	// 복제는 서버에서 클라이언트로만 작동하기에 해당 클라이언트 혼자만 보이는 것이다.
	// 이 때 문제점으로 서버 플레이어는 위젯을 얻지 못한다.(서버에서 클라이언트로만 작동하는 함수라서)
	

}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			SubSystem->AddMappingContext(DefaultContext, 0);
	}
	
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
		EnhancedInputComponent->BindAction(IA_EquipWeapon, ETriggerEvent::Started, this, &ABlasterCharacter::EquipButtonPressed);
	}
}




void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	//Endoverlap 되면 사라지면서 지울 수 있도록
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}



void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void ABlasterCharacter::EquipButtonPressed(const FInputActionValue& Value)
{
	//서버에서만 가능하게 해야하므로, HasAuthority()를 추가
	if(CombatComponent)
	{
		if(HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			Server_EquipButtonPressed();
		}
	}
}

//RPC 함수에 대해서
//정의를 할 때에는 해당 함수 이름의 뒤에 _Implementation 을 추가해준다.
//실행할 때에는 그냥 Server_EquipButtonPressed(); 이렇게 실행하면 된다.
void ABlasterCharacter::Server_EquipButtonPressed_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	//이 함수가 true가 나온다면 해당 플레이어는 Host로 ListenServer를 연 플레이어 란 것을 알 수 있음
	//그 안에는 Rep_Notify에 대한 로직이 들어가면 될 것 같다.
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}
