// Shoot Them Up Game, All Rights Reserved.


#include "Components/STURespawnComponent.h"
#include "STUGameModeBase.h"

// Sets default values for this component's properties
USTURespawnComponent::USTURespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
/// <summary>
/// 重生定时器
/// </summary>
/// <param name="RespawnTime"></param>
void USTURespawnComponent::Respawn(int32 RespawnTime)
{
    if (!GetWorld()) return;

	RespawnCountDown = RespawnTime;
    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &USTURespawnComponent::RespawnTimerUpdate, 1.0f, true);
}
/// <summary>
/// 重生时间倒计时
/// </summary>
void USTURespawnComponent::RespawnTimerUpdate()
{
    if (--RespawnCountDown == 0)
    {
        if (!GetWorld()) return;
        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (!GameMode) return;

        GameMode->RespawnRequest(Cast<AController>(GetOwner()));
    }
}
/// <summary>
/// 是否正在等待复活
/// </summary>
/// <returns></returns>
bool USTURespawnComponent::IsRespawnInProgress() const
{
    return GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(RespawnTimerHandle);
}