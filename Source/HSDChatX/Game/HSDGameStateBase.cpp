#include "HSDGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HSDPlayerController.h"

void AHSDGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, CurrentTurnPlayerState);
    DOREPLIFETIME(ThisClass, TurnTimeRemaining);
    DOREPLIFETIME(ThisClass, bHasGuessedThisTurn);
}

void AHSDGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* TargetPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(TargetPC) == true)
		{
			AHSDPlayerController* HSDPC = Cast<AHSDPlayerController>(TargetPC);
			if (IsValid(HSDPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" 가 입장했습니다.");
				HSDPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}