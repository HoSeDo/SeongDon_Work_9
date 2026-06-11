#include "Player/HSDPlayerState.h"
#include "Net/UnrealNetwork.h"

AHSDPlayerState::AHSDPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void AHSDPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

// 💡 안전하게 const 함수로 유지 (헤더와 일치 확인 필요)
FString AHSDPlayerState::GetPlayerInfoString() const
{
	return FString::Printf(TEXT("%s(%d/%d)"), *PlayerNameString, CurrentGuessCount, MaxGuessCount);
}