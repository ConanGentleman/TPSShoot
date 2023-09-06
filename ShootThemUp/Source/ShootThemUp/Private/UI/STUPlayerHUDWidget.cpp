// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"
#include "Components/ProgressBar.h"
#include "Player/STUPlayerState.h"
/// <summary>
/// ��ʼ���ɹ�����
/// </summary>
void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        //GetOnNewPawnNotifier����Pawn�ı�ʱ���õ�θʹ
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);//��ֹ�������������pawn��� ע���ί��ʧЧ
        OnNewPawn(GetOwningPlayerPawn());//��һ����ʾ����
    }
}
void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(NewPawn);
    if (HealthComponent && !HealthComponent->OnHealthChanged.IsBoundToObject(this))//������ ����ض�����İ��Ƿ����
    {
        //ע���������ί��
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }

    UpdateHealthBar();
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health, float HealthDelta)
{
    if (HealthDelta<0.0f)
    {
        OnTakeDamage();

        if (!IsAnimationPlaying(DamageAnimation))//��鶯���Ƿ��ڲ���
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
/// ���ӵ���������ʾΪ��λ �ٵ���0����
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
