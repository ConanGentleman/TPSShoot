// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUSpectatorWidget.h"
#include "STUUtils.h"
#include "Components/STURespawnComponent.h"

bool USTUSpectatorWidget::GetRespawnTime(int32& CountDownTime) const
{
    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(GetOwningPlayer());
    if (!RespawnComponent ||
        !RespawnComponent->IsRespawnInProgress()) return false;//���û�������û�����ڸ���

    CountDownTime = RespawnComponent->GetRespawnCountDown();//���ڸ���  ��ȡ�����ʱ
    return true;
}