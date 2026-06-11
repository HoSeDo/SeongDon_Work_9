#include "Game/HSDGameModeBase.h" // 💡 프로젝트 경로에 맞게 수정
#include "HSDGameStateBase.h"
#include "Player/HSDPlayerController.h"
#include "Player/HSDPlayerState.h"
#include "EngineUtils.h"

void AHSDGameModeBase::OnPostLogin(AController* NewPlayer)
{
   Super::OnPostLogin(NewPlayer);
    
   AHSDPlayerController* PlayerController = Cast<AHSDPlayerController>(NewPlayer);
   if (IsValid(PlayerController))
   {
      PlayerController->NotificationText = FText::FromString(TEXT("게임서버에 연결되었습니다."));
      AllPlayerControllers.Add(PlayerController);

      AHSDPlayerState* HDPS = PlayerController->GetPlayerState<AHSDPlayerState>();
      if (IsValid(HDPS))
      {
         HDPS->PlayerNameString = TEXT("플레이어") + FString::FromInt(AllPlayerControllers.Num());
      }

      AHSDGameStateBase* HSDGS = GetGameState<AHSDGameStateBase>();
      if (IsValid(HSDGS) && IsValid(HDPS))
      {
         HSDGS->MulticastRPCBroadcastLoginMessage(HDPS->PlayerNameString);
         if (AllPlayerControllers.Num() == 2)
         {
            HSDGS->CurrentTurnPlayerState = AllPlayerControllers[0]->GetPlayerState<AHSDPlayerState>();
            StartNewTurn();
         }
      }
   }
}

void AHSDGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    SecretNumberString = GenerateSecretNumber();
    
    UE_LOG(LogTemp, Warning, TEXT("========================================="));
    UE_LOG(LogTemp, Warning, TEXT("[SERVER] 생성된 정답 숫자: %s"), *SecretNumberString);
    UE_LOG(LogTemp, Warning, TEXT("========================================="));
}

void AHSDGameModeBase::PrintChatMessageString(AHSDPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
    if (!IsValid(InChattingPlayerController)) return;

    FString CleanedMessageString = InChatMessageString.TrimStartAndEnd();
    AHSDPlayerState* ChattingPS = InChattingPlayerController->GetPlayerState<AHSDPlayerState>();
    AHSDGameStateBase* HSDGS = GetGameState<AHSDGameStateBase>();
   
    if (CleanedMessageString.Len() == 3 && IsGuessNumberString(CleanedMessageString))
    {
        if (IsValid(HSDGS) && IsValid(ChattingPS))
        {
            if (HSDGS->TurnTimeRemaining <= 0 || HSDGS->CurrentTurnPlayerState != ChattingPS)
            {
                InChattingPlayerController->NotificationText = FText::FromString(TEXT("지금은 숫자를 입력할 수 없습니다! (턴/시간 확인)"));
                return;
            }
        }
       
        if (IsValid(HSDGS)) HSDGS->bHasGuessedThisTurn = true;

        // 시도 횟수 차감 및 닉네임 결합
        IncreaseGuessCount(InChattingPlayerController);
        FString PlayerPrefix = IsValid(ChattingPS) ? ChattingPS->GetPlayerInfoString() + TEXT(": ") : TEXT("Unknown: ");
        FString JudgeResultString = JudgeResult(SecretNumberString, CleanedMessageString);
        FString CombinedMessageString = PlayerPrefix + CleanedMessageString + TEXT(" -> ") + JudgeResultString;
        
        for (TActorIterator<AHSDPlayerController> It(GetWorld()); It; ++It)
        {
            if (IsValid(*It)) { (*It)->ClientRPCPrintChatMessageString(CombinedMessageString); }
        }

        // 스트라이크 판정
        int32 StrikeCount = 0;
        for (int32 i = 0; i < 3; ++i) { if (SecretNumberString[i] == CleanedMessageString[i]) StrikeCount++; }
        
        if (StrikeCount == 3)
        {
            JudgeGame(InChattingPlayerController, StrikeCount);
        }
        else
        {
            AdvanceTurn();
        }
    }
    else
    {
        // 일반 채팅은 턴/시간 제약 없이 상시 허용
        FString PlayerPrefix = IsValid(ChattingPS) ? ChattingPS->GetPlayerInfoString() + TEXT(": ") : TEXT("Unknown: ");
        FString CombinedMessageString = PlayerPrefix + InChatMessageString;
        for (TActorIterator<AHSDPlayerController> It(GetWorld()); It; ++It)
        {
            if (IsValid(*It)) { (*It)->ClientRPCPrintChatMessageString(CombinedMessageString); }
        }
    }
}

void AHSDGameModeBase::StartNewTurn()
{
   AHSDGameStateBase* HSDGS = GetGameState<AHSDGameStateBase>();
   if (!IsValid(HSDGS)) return;

   // 변수 초기화 및 타이머 셋업
   HSDGS->TurnTimeRemaining = MaxTurnTime;
   HSDGS->bHasGuessedThisTurn = false;

   // 1초마다 UpdateTurnTimer 함수를 실행하는 타이머 가동
   GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &AHSDGameModeBase::UpdateTurnTimer, 1.f, true);

   // 공지 UI 갱신
   if (IsValid(HSDGS->CurrentTurnPlayerState))
   {
      FString Announce = HSDGS->CurrentTurnPlayerState->PlayerNameString + TEXT(" 님의 턴 시작! (제한시간 30초)");
      for (AHSDPlayerController* PC : AllPlayerControllers) 
      { 
         if (IsValid(PC)) PC->NotificationText = FText::FromString(Announce); 
      }
   }
}

void AHSDGameModeBase::UpdateTurnTimer()
{
   AHSDGameStateBase* HSDGS = GetGameState<AHSDGameStateBase>();
   if (!IsValid(HSDGS)) return;

   HSDGS->TurnTimeRemaining--;
   
   if (HSDGS->TurnTimeRemaining <= 0)
   {
      HandleTimeOut();
   }
}

void AHSDGameModeBase::HandleTimeOut()
{
   GetWorldTimerManager().ClearTimer(TurnTimerHandle);

   AHSDGameStateBase* HSDGS = GetGameState<AHSDGameStateBase>();
   if (IsValid(HSDGS) && IsValid(HSDGS->CurrentTurnPlayerState))
   {
      // 활동 체크: 해당 턴에 숫자를 입력하지 않았다면 강제로 기회(시도 횟수) 1회 패널티 차감!
      if (HSDGS->bHasGuessedThisTurn == false)
      {
         for (AHSDPlayerController* PC : AllPlayerControllers)
         {
            if (IsValid(PC) && PC->GetPlayerState<AHSDPlayerState>() == HSDGS->CurrentTurnPlayerState)
            {
               IncreaseGuessCount(PC);
            
               FString PenaltyMsg = HSDGS->CurrentTurnPlayerState->PlayerNameString + TEXT(" 님이 시간 초과로 기회를 1회 상실했습니다.");
               for (AHSDPlayerController* EachPC : AllPlayerControllers) 
               { 
                  if (IsValid(EachPC)) EachPC->ClientRPCPrintChatMessageString(PenaltyMsg); 
               }
               break;
            }
         }
      }
   }
   AdvanceTurn();
}

void AHSDGameModeBase::AdvanceTurn()
{
   GetWorldTimerManager().ClearTimer(TurnTimerHandle);

   AHSDGameStateBase* HSDGS = GetGameState<AHSDGameStateBase>();
   if (!IsValid(HSDGS) || AllPlayerControllers.Num() == 0) return;
   
   if (HSDGS->CurrentTurnPlayerState == nullptr && AllPlayerControllers.Num() > 0)
   {
      HSDGS->CurrentTurnPlayerState = AllPlayerControllers[0]->GetPlayerState<AHSDPlayerState>();
      StartNewTurn();
      return;
   }
   
   int32 CurrentIndex = -1;
   for (int32 i = 0; i < AllPlayerControllers.Num(); ++i)
   {
      if (IsValid(AllPlayerControllers[i]) && AllPlayerControllers[i]->GetPlayerState<AHSDPlayerState>() == HSDGS->CurrentTurnPlayerState)
      {
         CurrentIndex = i;
         break;
      }
   }
   
   int32 NextIndex = (CurrentIndex + 1) % AllPlayerControllers.Num();
   
   if (IsValid(AllPlayerControllers[NextIndex]))
   {
      AHSDPlayerState* NextPS = AllPlayerControllers[NextIndex]->GetPlayerState<AHSDPlayerState>();
      if (IsValid(NextPS))
      {
         HSDGS->CurrentTurnPlayerState = NextPS;
         
         FString TurnAnnounce = NextPS->PlayerNameString + TEXT(" 님의 차례입니다!");
         
         for (AHSDPlayerController* PC : AllPlayerControllers)
         {
            if (IsValid(PC)) 
            {
               PC->NotificationText = FText::FromString(TurnAnnounce);
            }
         }
         StartNewTurn();
      }
   }
}

FString AHSDGameModeBase::GenerateSecretNumber()
{
    TArray<int32> Numbers;
    for (int32 i = 1; i <= 9; ++i)
    {
       Numbers.Add(i);
    }
   
    FString Result;
    for (int32 i = 0; i < 3; ++i)
    {
       int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
       Result.Append(FString::FromInt(Numbers[Index]));
       Numbers.RemoveAt(Index);
    }

    return Result;
}

bool AHSDGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
    if (InNumberString.Len() != 3) return false;

    TSet<TCHAR> UniqueDigits;
    for (TCHAR C : InNumberString)
    {
       if (FChar::IsDigit(C) == false || C == '0')
       {
          return false;
       }
       UniqueDigits.Add(C);
    }

    if (UniqueDigits.Num() != 3)
    {
       return false;
    }

    return true;
}

FString AHSDGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
    int32 StrikeCount = 0, BallCount = 0;

    for (int32 i = 0; i < 3; ++i)
    {
       if (InSecretNumberString[i] == InGuessNumberString[i])
       {
          StrikeCount++;
       }
       else 
       {
          FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
          if (InSecretNumberString.Contains(PlayerGuessChar))
          {
             BallCount++;            
          }
       }
    }

    if (StrikeCount == 0 && BallCount == 0)
    {
       return TEXT("아웃");
    }

    return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AHSDGameModeBase::IncreaseGuessCount(AHSDPlayerController* InChattingPlayerController)
{
    if (!IsValid(InChattingPlayerController)) return;
    
    AHSDPlayerState* HDPS = InChattingPlayerController->GetPlayerState<AHSDPlayerState>();
    if (IsValid(HDPS) == true)
    {
       HDPS->CurrentGuessCount++;
    }
}

void AHSDGameModeBase::ResetGame()
{
   SecretNumberString = GenerateSecretNumber();
   UE_LOG(LogTemp, Warning, TEXT("[SERVER] 게임 리셋! 새로운 정답 숫자: %s"), *SecretNumberString);
   
   for (AHSDPlayerController* HDPlayerController : AllPlayerControllers)
   {
      if (IsValid(HDPlayerController))
      {
         AHSDPlayerState* HDPS = HDPlayerController->GetPlayerState<AHSDPlayerState>();
         if (IsValid(HDPS))
         {
            HDPS->CurrentGuessCount = 0;
         }
      }
   }
}

void AHSDGameModeBase::JudgeGame(AHSDPlayerController* InChattingPlayerController, int InStrikeCount)
{
   if (!IsValid(InChattingPlayerController)) return;

   if (3 == InStrikeCount)
   {
      AHSDPlayerState* HDPS = InChattingPlayerController->GetPlayerState<AHSDPlayerState>();
      if (IsValid(HDPS))
      {
         FString CombinedMessageString = HDPS->PlayerNameString + TEXT(" 님이 승리하였습니다!");
          
         // 💡 타입을 명시적으로 수정
         for (AHSDPlayerController* HDPlayerController : AllPlayerControllers)
         {
            if (IsValid(HDPlayerController))
            {
               HDPlayerController->NotificationText = FText::FromString(CombinedMessageString);
            }
         }
         ResetGame();
      }
   }
   
   else
   {
      bool bIsDraw = true;
      // 💡 타입을 명시적으로 수정
      for (AHSDPlayerController* HDPlayerController : AllPlayerControllers)
      {
         if (IsValid(HDPlayerController))
         {
            AHSDPlayerState* HDPS = HDPlayerController->GetPlayerState<AHSDPlayerState>();
            if (IsValid(HDPS))
            {
               if (HDPS->CurrentGuessCount < HDPS->MaxGuessCount)
               {
                  bIsDraw = false;
                  break;
               }
            }
         }
      }

      if (bIsDraw == true)
      {
         for (AHSDPlayerController* HDPlayerController : AllPlayerControllers)
         {
            if (IsValid(HDPlayerController))
            {
               HDPlayerController->NotificationText = FText::FromString(TEXT("모든 기회 소진! 무승부..."));
            }
         }
         ResetGame();
      }
   }
}