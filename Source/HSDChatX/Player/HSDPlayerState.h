#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HSDPlayerState.generated.h"

UCLASS()
class HSDCHATX_API AHSDPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AHSDPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString() const;
public:
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "PlayerInfo")
	FString PlayerNameString;
	
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	
	
};
