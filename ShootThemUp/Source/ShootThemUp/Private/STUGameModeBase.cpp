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
/// 生成AI
/// </summary>
void ASTUGameModeBase::SpawnBots() 
{
    if (!GetWorld()) return;

    for (int32 i = 0; i < GameData.PlayersNum - 1; ++i)
    {
        FActorSpawnParameters SpawnInfo;
        //解决生成时 碰撞器冲突
        SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        //直接生成控制器类 参数：控制器类；SpawnInfo结构
        const auto STUAIController=GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
        //创建pawn(生成玩家）
        RestartPlayer(STUAIController);
    }
}

void ASTUGameModeBase::StartRound() 
{
    RoundCountDown = GameData.RoundTime;
    //每秒调用1次
    GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ASTUGameModeBase::GameTimerUpdate, 1.0f, true);
}

void ASTUGameModeBase::GameTimerUpdate() 
{
    //UE_LOG(LogSTUGameModeBase, Display, TEXT("Timer: %i / Round: %i/%i"),RoundCountDown,CurrentRound,GameData.RoundsNum);

    //const auto TimerRate = GetWorldTimerManager().GetTimerRate(GameRoundTimerHandle);//类似Unity中的DeltaTime
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

    for (auto It = GetWorld()->GetControllerIterator(); It;++It) //获取世界中的所有控制器类
    {
        ResetOnePlayer(It->Get());
    }
}
/// <summary>
/// 生成玩家
/// </summary>
/// <param name="Controller"></param>
void ASTUGameModeBase::ResetOnePlayer(AController* Controller) 
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();//Reset会在满足条件的情况下调用Destory
    }
    //生成玩家
    RestartPlayer(Controller);
    SetPlayerColor(Controller);
}

/// <summary>
/// 甚至队伍信息
/// </summary>
void ASTUGameModeBase::CreateTeamsInfo()
{
    if (!GetWorld()) return;

    int32 TeamID = 1;
    for (auto It = GetWorld()->GetControllerIterator(); It;++It) //获取世界中的所有控制器类
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState) continue;

        PlayerState->SetTeamID(TeamID);
        PlayerState->SetTeamColor(DetermineColorByTeamID(TeamID));
        PlayerState->SetPlayerName(Controller->IsPlayerController() ? "Player" : "Bot");
        SetPlayerColor(Controller);
        TeamID = TeamID == 1 ? 2 : 1;//循环设置ID
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

    //UE_LOG(LogSTUGameModeBase, Display, TEXT("设置颜色"));
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

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It) //获取世界中的所有控制器类
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState)continue;

        PlayerState->LogInfo();
    }

}

/// <summary>
/// 开始复活倒计时
/// </summary>
/// <param name="Controller"></param>
void ASTUGameModeBase::StartRespawn(AController* Controller)
{
    const auto RespawnAvailable = RoundCountDown> MinRoundTimeForRespawn+GameData.RespawnTime;//如果倒计时时间大于了重生时间+偏差 则启动重生（否则等待至下一轮）
    if (!RespawnAvailable) return;

    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(Controller);
    if (!RespawnComponent) return;

    RespawnComponent->Respawn(GameData.RespawnTime);
}

///复活生成玩家
void ASTUGameModeBase::RespawnRequest(AController* Controller) 
{
    ResetOnePlayer(Controller);
}
/// <summary>
/// 游戏结束 暂停所有actor移动
/// </summary>
void ASTUGameModeBase::GameOver()
{
    //UE_LOG(LogSTUGameModeBase, Display, TEXT("====== GAME OVER ======"));
    LogPlayerInfo();

    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff(); //停止移动碰撞
            Pawn->DisableInput(nullptr); //停止输入

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
/// 暂停游戏
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