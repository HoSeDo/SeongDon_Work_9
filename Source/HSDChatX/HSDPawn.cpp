#include "HSDPawn.h"
#include "HSDChatX.h"

AHSDPawn::AHSDPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHSDPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = ChatXFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *ChatXFunctionLibrary::GetNetModeString(this), *NetRoleString);
	ChatXFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void AHSDPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = ChatXFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *ChatXFunctionLibrary::GetNetModeString(this), *NetRoleString);
	ChatXFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}




