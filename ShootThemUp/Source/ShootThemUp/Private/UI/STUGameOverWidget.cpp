// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUGameOverWidget.h"
#include "STUGameModeBase.h"
#include "Player/STUPlayerState.h"
#include "UI/STUPlayerStatRowWidget.h"
#include "Components/VerticalBox.h"
#include "STUUtils.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

/// <summary>
/// 初始化成功调用该方法
/// </summary>
void USTUGameOverWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            //订阅游戏状态变更委托
            GameMode->OnMatchStateChanged.AddUObject(this, &USTUGameOverWidget::OnMatchStateChanged);
        }
    }

    if (ResetLevelButton)
    {
        ResetLevelButton->OnClicked.AddDynamic(this, &USTUGameOverWidget::OnResetLevel);
    }
}
void USTUGameOverWidget::OnMatchStateChanged(ESTUMatchState State)
{
    if (State ==ESTUMatchState::GameOver)
    {
        UpdatePlayersState();
    }
}
void USTUGameOverWidget::UpdatePlayersState()
{
    if(!GetWorld() || !PlayerStatBox) return;

    PlayerStatBox->ClearChildren();

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState) continue;

        const auto PlayerStatRowWidget = CreateWidget<USTUPlayerStatRowWidget>(GetWorld(), PlayerStatRowWidgetClass);
        if (!PlayerStatRowWidget) continue;

        PlayerStatRowWidget->SetPlayerName(FText::FromString(PlayerState->GetPlayerName()));
        PlayerStatRowWidget->SetKills(STUUtils::TextFormInt(PlayerState->GetKillsNum()));
        PlayerStatRowWidget->SetDeaths(STUUtils::TextFormInt(PlayerState->GetDeathsNum()));
        PlayerStatRowWidget->SetTeam(STUUtils::TextFormInt(PlayerState->GetTeamID()));
        PlayerStatRowWidget->SetPlayerIndicatorVisibility(Controller->IsPlayerController());
        PlayerStatRowWidget->SetTeamColor(PlayerState->GetTeamColor());

        PlayerStatBox->AddChild(PlayerStatRowWidget);
    }
}
/// <summary>
/// 重新开始游戏
/// </summary>
void USTUGameOverWidget::OnResetLevel()
{
    //const FName CurrentLevelName = "TestLevel";
    const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
    //加载场景（重新开始游戏的方式之一：硬重置），另一种方法-软重置：调用ResetLevel，但这种方法需要为所有的类写Reset()方法，较麻烦） 
    //参数：this或GetWorld()；场景名称；其余默认
    UGameplayStatics::OpenLevel(this, FName(CurrentLevelName));
}