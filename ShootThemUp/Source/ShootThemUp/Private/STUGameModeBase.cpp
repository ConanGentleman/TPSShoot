// Shoot Them Up Game, All Rights Reserved.


#include "STUGameModeBase.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "UI/STUGameHUD.h"
#include "AIController.h"
#include "Player/STUPlayerState.h"
#include "STUUtils.h"
#include "Components/STURespawnComponent.h"
#include "Components/STUWeaponComponent.h"
#include "EngineUtils.h"
#include "STUGameInstance.h"

//DEFINE_LOG_CATEGORY_STATIC(LogSTUGameModeBase, All, All);

constexpr static int32 MinRoundTimeForRespawn = 10;

ASTUGameModeBase::ASTUGameModeBase() 
{
    DefaultPawnClass = ASTUBaseCharacter::StaticClass();
    PlayerControllerClass = ASTUPlayerController::StaticClass();
    HUDClass = ASTUGameHUD::StaticClass();
    PlayerStateClass = ASTUPlayerState::StaticClass();
}

void ASTUGameModeBase::StartPlay()
{
    Super::StartPlay();

    SpawnBots();
    CreateTeamsInfo();

    CurrentRound = 1;
    StartRound();

    SetMatchState(ESTUMatchState::InProgress);
}
UClass* ASTUGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (InController && InController->IsA<AAIController>())
    {
        return AIPawnClass;
    }
    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

/// <summary>
/// ����AI
/// </summary>
void ASTUGameModeBase::SpawnBots() 
{
    if (!GetWorld()) return;

    for (int32 i = 0; i < GameData.PlayersNum - 1; ++i)
    {
        FActorSpawnParameters SpawnInfo;
        //�������ʱ ��ײ����ͻ
        SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        //ֱ�����ɿ������� �������������ࣻSpawnInfo�ṹ
        const auto STUAIController=GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
        //����pawn(������ң�
        RestartPlayer(STUAIController);
    }
}

void ASTUGameModeBase::StartRound() 
{
    RoundCountDown = GameData.RoundTime;
    //ÿ�����1��
    GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ASTUGameModeBase::GameTimerUpdate, 1.0f, true);
}

void ASTUGameModeBase::GameTimerUpdate() 
{
    //UE_LOG(LogSTUGameModeBase, Display, TEXT("Timer: %i / Round: %i/%i"),RoundCountDown,CurrentRound,GameData.RoundsNum);

    //const auto TimerRate = GetWorldTimerManager().GetTimerRate(GameRoundTimerHandle);//����Unity�е�DeltaTime
    //RoundCountDown -= TimerRate;

    if (--RoundCountDown == 0)
    {
        GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);

        if (CurrentRound + 1 <= GameData.RoundsNum)
        {
            ++CurrentRound;
            ResetPlayers();
            StartRound();
        }
        else
        {
            GameOver();
        }
    }
}

void ASTUGameModeBase::ResetPlayers() 
{
    if (!GetWorld()) return;

    for (auto It = GetWorld()->GetControllerIterator(); It;++It) //��ȡ�����е����п�������
    {
        ResetOnePlayer(It->Get());
    }
}
/// <summary>
/// �������
/// </summary>
/// <param name="Controller"></param>
void ASTUGameModeBase::ResetOnePlayer(AController* Controller) 
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();//Reset������������������µ���Destory
    }
    //�������
    RestartPlayer(Controller);
    SetPlayerColor(Controller);
}

/// <summary>
/// ����������Ϣ
/// </summary>
void ASTUGameModeBase::CreateTeamsInfo()
{
    if (!GetWorld()) return;

    int32 TeamID = 1;
    for (auto It = GetWorld()->GetControllerIterator(); It;++It) //��ȡ�����е����п�������
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState) continue;

        PlayerState->SetTeamID(TeamID);
        PlayerState->SetTeamColor(DetermineColorByTeamID(TeamID));
        PlayerState->SetPlayerName(Controller->IsPlayerController() ? "Player" : "Bot");
        SetPlayerColor(Controller);
        TeamID = TeamID == 1 ? 2 : 1;//ѭ������ID
    }
}
FLinearColor ASTUGameModeBase::DetermineColorByTeamID(int32 TeamID) const
{
    if (TeamID - 1 < GameData.TeamColors.Num())
    {
        return GameData.TeamColors[TeamID - 1];
    }
    return GameData.DefaultTeamColor;
}
void ASTUGameModeBase::SetPlayerColor(AController* Controller)
{
    if (!Controller) return;

    const auto Character=Cast<ASTUBaseCharacter>(Controller->GetPawn());
    if (!Character) return;

    const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
    if (!PlayerState) return;

    //UE_LOG(LogSTUGameModeBase, Display, TEXT("������ɫ"));
    Character->SetPlayerColor(PlayerState->GetTeamColor());
}
void ASTUGameModeBase::Killed(AController* KillerController, AController* VictimController) 
{
    const auto KillerPlayerState = KillerController ? Cast<ASTUPlayerState>(KillerController->PlayerState) : nullptr;
    const auto VictimPlayerState = VictimController ? Cast<ASTUPlayerState>(VictimController->PlayerState) : nullptr;

    if (KillerPlayerState)
    {
        KillerPlayerState->AddKill();
    }

    if (VictimPlayerState)
    {
        VictimPlayerState->AddDeath();
    }

    StartRespawn(VictimController);
}
void ASTUGameModeBase::LogPlayerInfo() 
{
    if (!GetWorld()) return;

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It) //��ȡ�����е����п�������
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState)continue;

        PlayerState->LogInfo();
    }

}

/// <summary>
/// ��ʼ�����ʱ
/// </summary>
/// <param name="Controller"></param>
void ASTUGameModeBase::StartRespawn(AController* Controller)
{
    const auto RespawnAvailable = RoundCountDown> MinRoundTimeForRespawn+GameData.RespawnTime;//�������ʱʱ�����������ʱ��+ƫ�� ����������������ȴ�����һ�֣�
    if (!RespawnAvailable) return;

    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(Controller);
    if (!RespawnComponent) return;

    RespawnComponent->Respawn(GameData.RespawnTime);
}

///�����������
void ASTUGameModeBase::RespawnRequest(AController* Controller) 
{
    ResetOnePlayer(Controller);
}
/// <summary>
/// ��Ϸ���� ��ͣ����actor�ƶ�
/// </summary>
void ASTUGameModeBase::GameOver()
{
    //UE_LOG(LogSTUGameModeBase, Display, TEXT("====== GAME OVER ======"));
    LogPlayerInfo();

    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff(); //ֹͣ�ƶ���ײ
            Pawn->DisableInput(nullptr); //ֹͣ����

        }
    }

    SetMatchState(ESTUMatchState::GameOver);
}

void ASTUGameModeBase::SetMatchState(ESTUMatchState State)
{
    if (MatchState == State) return;

    MatchState = State;
    OnMatchStateChanged.Broadcast(MatchState);
}
/// <summary>
/// ��ͣ��Ϸ
/// </summary>
/// <param name="PC"></param>
/// <param name="CanUnpauseDelegate"></param>
/// <returns></returns>
bool ASTUGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
    const auto PauseSet = Super::SetPause(PC, CanUnpauseDelegate);

    if (PauseSet)
    {
        SetMatchState(ESTUMatchState::Pause);
    }
    
    return PauseSet;
}

bool ASTUGameModeBase::ClearPause() 
{
    const auto PauseCleared = Super::ClearPause();
    if (PauseCleared)
    {
        StopAllFire();
        SetMatchState(ESTUMatchState::InProgress);
    }
    return PauseCleared;
}


void ASTUGameModeBase::StopAllFire()
{
    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(Pawn);
        if (!WeaponComponent) continue;

        WeaponComponent->StopFire();
        WeaponComponent->Zoom(false);
    }
}