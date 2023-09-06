// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUAIWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"

void USTUAIWeaponComponent::StartFire()
{
    if (!CanFire()) return;

    if (CurrentWeapon->IsAmmoEmpty())
    {
        NextWeapon();//AIû�ӵ��ͻ���һ������
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

    if (CurrentWeaponIndex != NextIndex)//ѭ������ û�лص����������  �����ҵ����е�ϻ������
    {
        CurrentWeaponIndex = NextIndex;
        EquipWeapon(CurrentWeaponIndex);
    }
}