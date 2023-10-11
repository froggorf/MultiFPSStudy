// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"


void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	//LocalRole -> 호출하는 컴퓨터마다 다름
	ENetRole LocalRole = InPawn->GetLocalRole();
	//RemoteRole -> 반대로 되는데 아직은 파악이 안됨.
	//ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		//StandAlone 이거나 Server일 때
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		//클라이언트 당사자
		Role = FString("AutonomousProxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		//타 클라이언트
		Role = FString("SimulatedProxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	SetDisplayText(LocalRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
