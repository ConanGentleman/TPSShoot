// Shoot Them Up Game, All Rights Reserved.


#include "Player/STUPlayerController.h"
#include "Components/STURespawnComponent.h"
#include "STUGameModeBase.h"
#include "STUGameInstance.h"

ASTUPlayerController::ASTUPlayerController()
{
    RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("RespawnComponent");
}

void ASTUPlayerController::BeginPlay() 
{
    Super::BeginPlay();
    if (GetWorld())
    {
        
        if (const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode()))
        {
            //订阅委托
            GameMode->OnMatchStateChanged.AddUObject(this, &ASTUPlayerController::OnMatchStateChanged);
        }
    }
}
/// <summary>
/// 设置光标显示
/// </summary>
/// <param name="State"></param>
void ASTUPlayerController::OnMatchStateChanged(ESTUMatchState State)
{
    if (State == ESTUMatchState::InProgress)
    {
        SetInputMode(FInputModeGameOnly());//设置光标模式（处理UI出现时还需点一下游戏内才能进行交互的问题）
        //关闭光标
        bShowMouseCursor = false;
    }
    else
    {
        SetInputMode(FInputModeUIOnly());//设置光标模式
        //开启光标
        bShowMouseCursor = true;
    }
}
void ASTUPlayerController::OnPossess(APawn* InPawn) 
{
    Super::OnPossess(InPawn);

    OnNewPawn.Broadcast(InPawn);
}

void ASTUPlayerController::SetupInputComponent() 
{
    Super::SetupInputComponent();
    if(!InputComponent) return;

    InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASTUPlayerController::OnPauseGame);
    InputComponent->BindAction("Mute", IE_Pressed, this, &ASTUPlayerController::OnMuteSound);
}

void ASTUPlayerController::OnPauseGame() 
{
    if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;

    //暂停游戏  参数：暂停游戏的控制器；是否暂停（使用默认值）
    GetWorld()->GetAuthGameMode()->SetPause(this);
}

void ASTUPlayerController::OnMuteSound()
{
    if (!GetWorld()) return;
    const auto STUGameInstance = GetWorld() -> GetGameInstance<USTUGameInstance>();
    if (!STUGameInstance) return;

    STUGameInstance->ToggleVolume();
}
