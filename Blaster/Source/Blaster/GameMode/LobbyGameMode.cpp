// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("일단 접속함"));
	//게임에 참가한 각 플레이어 상태 저장된 배열
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if(NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if(World)
		{
			//bUseSeamlessTravel 에 대한 설명 https://docs.unrealengine.com/4.27/ko/InteractiveExperiences/Networking/Travelling/
			bUseSeamlessTravel = true;

			// level?listen -> 리슨서버로 열기
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}

}
