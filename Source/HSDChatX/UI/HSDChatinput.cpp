#include "HSDChatinput.h"

#include "Components/EditableTextBox.h"
#include "Player/HSDPlayerController.h"

void UHSDChatinput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_HSDChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_HSDChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);		
	}	
}

void UHSDChatinput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_HSDChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_HSDChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UHSDChatinput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			AHSDPlayerController* OwningCXPlayerController = Cast<AHSDPlayerController>(OwningPlayerController);
			if (IsValid(OwningCXPlayerController) == true)
			{
				OwningCXPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_HSDChatInput->SetText(FText());
			}
		}
	}
}