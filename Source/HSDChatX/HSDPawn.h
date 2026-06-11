#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HSDPawn.generated.h"

UCLASS()
class HSDCHATX_API AHSDPawn : public APawn
{
	GENERATED_BODY()

public:
	AHSDPawn();

protected:
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;

};
