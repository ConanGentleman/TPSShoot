// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"
#include "Components/ProgressBar.h"
#include "Player/STUPlayerState.h"
/// <summary>
/// 初始化成功调用
/// </summary>
void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        //GetOnNewPawnNotifier用于Pawn改变时调用的胃痛
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);//防止玩家重生后由于pawn变更 注册的委托失效
        OnNewPawn(GetOwningPlayerPawn());//第一次显示调用
    }
}
void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(NewPawn);
    if (HealthComponent && !HealthComponent->OnHealthChanged.IsBoundToObject(this))//后者是 检测特定对象的绑定是否存在
    {
        //注册声明变更委托
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }

    UpdateHealthBar();
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health, float HealthDelta)
{
    if (HealthDelta<0.0f)
    {
        OnTakeDamage();

        if (!IsAnimationPlaying(DamageAnimation))//检查动画是否在播放
        {
            PlayAnimation(DamageAnimation);
        }
    }
   
    UpdateHealthBar();
}

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    //const auto HealthComponent = GetHealthComponent();
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (!HealthComponent) return 0.0f;

    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
    //const auto WeaponComponent = GetWeaponComponent();
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent) return false;

    return WeaponComponent->GetCurrentWeaponUIData(UIData);
}

bool USTUPlayerHUDWidget::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
    //const auto WeaponComponent = GetWeaponComponent();
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent) return false;

    return WeaponComponent->GetCurrentWeaponAmmoData(AmmoData);
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
   // const auto HealthComponent = GetHealthComponent();
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    return HealthComponent && !HealthComponent->IsDead();
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{
    const auto Controller = GetOwningPlayer();
    return Controller && Controller->GetStateName() == NAME_Spectating;
}

int32 USTUPlayerHUDWidget::GetKillsNum() const
{
    const auto Controller = GetOwningPlayer();
    if (!Controller) return 0;

    const auto PlayerState = Cast<ASTUPlayerState>(Controller -> PlayerState);
    return PlayerState ? PlayerState->GetKillsNum() : 0;
}

void USTUPlayerHUDWidget::UpdateHealthBar()
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetFillColorAndOpacity(GetHealthPercent() > PercentColorThreshold ? GoodColor : BadColor);
    }
}
/// <summary>
/// 让子弹的数量显示为两位 少的用0补足
/// </summary>
/// <param name="BulletsNum"></param>
/// <returns></returns>
FString USTUPlayerHUDWidget::FormatBullets(int32 BulletsNum) const
{
    const int32 MaxLen = 3;
    const TCHAR PrefixSymbol = '0';

    auto BulletStr = FString::FromInt(BulletsNum);
    const auto SymbolsNumToAdd = MaxLen - BulletStr.Len();

    if (SymbolsNumToAdd > 0)
    {
        BulletStr = FString::ChrN(SymbolsNumToAdd, PrefixSymbol).Append(BulletStr);
    }

    return BulletStr;
}
//USTUWeaponComponent* USTUPlayerHUDWidget::GetWeaponComponent() const 
//{
//    const auto Player = GetOwningPlayerPawn();
//    if (!Player) return nullptr;
//
//    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass());
//    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
//    
//    return WeaponComponent;
//}
//
//USTUHealthComponent* USTUPlayerHUDWidget::GetHealthComponent() const
//{
//    const auto Player = GetOwningPlayerPawn();
//    if (!Player) return nullptr;
//
//    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass());
//    const auto HealthComponent = Cast<USTUHealthComponent>(Component);
//    
//    return HealthComponent;
//}
