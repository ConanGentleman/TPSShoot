// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUAIWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"

void USTUAIWeaponComponent::StartFire()
{
    if (!CanFire()) return;

    if (CurrentWeapon->IsAmmoEmpty())
    {
        NextWeapon();//AI没子弹就换下一把武器
    }
    else
    {
        CurrentWeapon->StartFire();
    }
}
void USTUAIWeaponComponent::NextWeapon() 
{
    if (!CanEquip()) return;

    int NextIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    while (NextIndex != CurrentWeaponIndex)
    {
        if (!Weapons[NextIndex]->IsAmmoEmpty()) break;
        NextIndex = (NextIndex + 1) % Weapons.Num();
    }

    if (CurrentWeaponIndex != NextIndex)//循环遍历 没有回到最初的索引  表明找到了有弹匣的武器
    {
        CurrentWeaponIndex = NextIndex;
        EquipWeapon(CurrentWeaponIndex);
    }
}