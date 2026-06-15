#include "Player/HSDPlayerController.h" // 💡 경로 확인 필요 시 수정
#include "HSDChatX.h"
#include "Game/HSDGameModeBase.h"
#include "Player/HSDPlayerState.h"
#include "UI/HSDChatinput.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AHSDPlayerController::AHSDPlayerController()
{
    bReplicates = true;
}

void AHSDPlayerController::BeginPlay()
{
	Super::BeginPlay();
    
	if (IsLocalController() == false) return; //
	
	FInputModeGameAndUI InputModeData;
	
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	bShowMouseCursor = true;

	if (IsValid(ChatInputWidgetClass) == true) //
	{
		ChatInputWidgetInstance = CreateWidget<UHSDChatinput>(this, ChatInputWidgetClass); //
		if (IsValid(ChatInputWidgetInstance) == true) //
		{
			ChatInputWidgetInstance->AddToViewport(); //
		}
	}
    
	if (IsValid(NotificationTextWidgetClass) == true) //
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass); //
		if (IsValid(NotificationTextWidgetInstance) == true) //
		{
			NotificationTextWidgetInstance->AddToViewport(); //
		}
	}
}

void AHSDPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
    ChatXFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AHSDPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
    ChatMessageString = InChatMessageString;
	
    	if (IsLocalController() == true)
    	{
    		ServerRPCPrintChatMessageString(InChatMessageString);
    	}
}

void AHSDPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
    PrintChatMessageString(InChatMessageString);
}

void AHSDPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
    AGameModeBase* CurrentGM = UGameplayStatics::GetGameMode(this);
    if (IsValid(CurrentGM) == true)
    {

       AHSDGameModeBase* HSDGameMode = Cast<AHSDGameModeBase>(CurrentGM);
       if (IsValid(HSDGameMode) == true)
       {
          HSDGameMode->PrintChatMessageString(this, InChatMessageString);
       }
    }
}

void AHSDPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, NotificationText);
}
