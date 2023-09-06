// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUPauseWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Button.h"
/// <summary>
/// 初始化成功调用
/// </summary>
/// <returns></returns>
void USTUPauseWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ClearPauseButton)
    {
        //注册按钮点击事件
        ClearPauseButton->OnClicked.AddDynamic(this, &USTUPauseWidget::OnClearPause);
    }
}

/// <summary>
/// 取消游戏暂停
/// </summary>
void USTUPauseWidget::OnClearPause()
{
    if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;

    GetWorld()->GetAuthGameMode()->ClearPause();
}