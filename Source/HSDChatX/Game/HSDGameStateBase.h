#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HSDGameStateBase.generated.h"

UCLASS()
class HSDCHATX_API AHSDGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// 현재 턴인 플레이어 (기존 변수 유지)
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Baseball|Turn")
    class AHSDPlayerState* CurrentTurnPlayerState;

    //서버에서 동기화할 남은 시간 (초 단위)
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Baseball|Turn")
    int32 TurnTimeRemaining;

    // 현재 플레이어가 이번 턴에 숫자를 입력했는지 체크하는 변수
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Baseball|Turn")
    bool bHasGuessedThisTurn;

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    // 멀티캐스트 RPC (기존 함수 유지)
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPCBroadcastLoginMessage(const FString& InNameString);
};
