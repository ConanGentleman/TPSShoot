// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUSpectatorWidget.h"
#include "STUUtils.h"
#include "Components/STURespawnComponent.h"

bool USTUSpectatorWidget::GetRespawnTime(int32& CountDownTime) const
{
    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(GetOwningPlayer());
    if (!RespawnComponent ||
        !RespawnComponent->IsRespawnInProgress()) return false;//如果没有组件或没有正在复活

    CountDownTime = RespawnComponent->GetRespawnCountDown();//正在复活  获取复活倒计时
    return true;
}