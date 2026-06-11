#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HSDGameModeBase.generated.h"

class AHSDPlayerController;

UCLASS()
class HSDCHATX_API AHSDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;	
	void PrintChatMessageString(AHSDPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	
	FString GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	void IncreaseGuessCount(AHSDPlayerController* InChattingPlayerController);
	void ResetGame();
	void JudgeGame(AHSDPlayerController* InChattingPlayerController, int InStrikeCount);
	
	void StartNewTurn();
	void AdvanceTurn();
	void UpdateTurnTimer(); // 1초마다 실행될 함수
	void HandleTimeOut();   // 0초가 되었을 때 실행될 함수

	FTimerHandle TurnTimerHandle;
    
	UPROPERTY(EditDefaultsOnly, Category = "Baseball|Rule")
	int32 MaxTurnTime = 30; // 턴당 제한 시간 (30초)
	
protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AHSDPlayerController>> AllPlayerControllers;
	
};
