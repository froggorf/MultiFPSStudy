// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//C++의 변수와 위젯 블프의 텍스트 블록을 연결해주는 기능
	//위젯 블프의 Text 의 이름이 UTextBlock* 변수명과 같아야함
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);
protected:
	//종료하거나 다른레벨로 갈 때 지워지도록 함
	virtual void NativeDestruct() override;
};
