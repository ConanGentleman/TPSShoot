// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUPauseWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Button.h"
/// <summary>
/// ��ʼ���ɹ�����
/// </summary>
/// <returns></returns>
void USTUPauseWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ClearPauseButton)
    {
        //ע�ᰴť����¼�
        ClearPauseButton->OnClicked.AddDynamic(this, &USTUPauseWidget::OnClearPause);
    }
}

/// <summary>
/// ȡ����Ϸ��ͣ
/// </summary>
void USTUPauseWidget::OnClearPause()
{
    if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;

    GetWorld()->GetAuthGameMode()->ClearPause();
}