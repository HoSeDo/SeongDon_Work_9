#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HSDPlayerController.generated.h"

class UHSDChatinput;

UCLASS()
class HSDCHATX_API AHSDPlayerController : public APlayerController
{
	GENERATED_BODY()
    
public:
	AHSDPlayerController();
    
	virtual void BeginPlay() override;
    
	void SetChatMessageString(const FString& InChatMessageString);
    
	void PrintChatMessageString(const FString& InChatMessageString);
    
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);
    
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// [채팅창 UI 관련 변수 클래스 및 인스턴스]
	UPROPERTY(EditDefaultsOnly, Category = "UI|Chat")
	TSubclassOf<UHSDChatinput> ChatInputWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UHSDChatinput> ChatInputWidgetInstance;
    
	FString ChatMessageString;
	
	// [공지사항 UI 관련 변수 클래스 및 인스턴스]
	UPROPERTY(EditDefaultsOnly, Category = "UI|Notification")
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
    
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;
    
public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Notification")
	FText NotificationText;
};
