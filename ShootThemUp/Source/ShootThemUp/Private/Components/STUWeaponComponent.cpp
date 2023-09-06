// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipFinishedAnimNotify.h"
#include "Animations/STUReloadFinishedAnimNotify.h"
#include "Animations/AnimUtil.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

// Sets default values for this component's properties
USTUWeaponComponent::USTUWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

constexpr static int32 WeaponNum = 2;

// Called when the game starts
void USTUWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

    checkf(WeaponData.Num() == WeaponNum, TEXT("Our character can hold only %i weapon items"), WeaponNum);

    CurrentWeaponIndex = 0;
    InitAnimations();
    SpawnWeapons();
    EquipWeapon(CurrentWeaponIndex);
}
void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //����������ٺ���������ҲӦ�ñ�����
    CurrentWeapon = nullptr;
    for (auto Weapon : Weapons)
    {
        //�������Ϸ���actor
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        //ɾ������
        Weapon->Destroy();
    }
    Weapons.Empty();
    Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || !GetWorld()) return;

    for (auto OneWeaponData : WeaponData)
    {
        //��������Actor
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
        if (!Weapon) continue;
        //����ί��
        Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnClipEmpty);
        //��������ӵ����
        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);

        AttachWeaponToSocket(Weapon,Character->GetMesh(),WeaponArmorySocketName);
    }

}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon,USkeletalMeshComponent* SceneComponent,const FName& SocketName) 
{
    if (!Weapon || !SceneComponent) return;
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false); //���ӹ���
    //��actor���ӵ����������� ���������ӵ��������AttachmentRules ���������
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
    if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num())
    {
        return;
    }

    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    if (CurrentWeapon)
    {
        //�رտ���
        CurrentWeapon->Zoom(false);
        //�л�����ֹͣ���
        CurrentWeapon->StopFire();
        //�������Żؽ�ɫ�ı���
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }

    CurrentWeapon = Weapons[WeaponIndex];
    //CurrentReloadAnimMontage = WeaponData[WeaponIndex].ReloadAnimMontage;
    //ͨ��CurrentWeapon�������Ӧ����̫�涯��
    const auto CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data){//
        return Data.WeaponClass==CurrentWeapon->GetClass();//
    });
    CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;

    //�л�������� ���ڽ�ɫ����
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
    EquipAnimInProgress = true;
    PlayAnimMontage(EquipAnimMontage);
}

void USTUWeaponComponent::StartFire()
{
    if (!CanFire()) return;
    CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopFire()
{
    if (!CurrentWeapon) return;
    CurrentWeapon->StopFire();
}
void USTUWeaponComponent::NextWeapon()
{
    if (!CanEquip()) return;
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}
/// <summary>
/// ������̫�涯��
/// </summary>
/// <param name="Animation"></param>
void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* Animation) 
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    Character->PlayAnimMontage(Animation);
}

void USTUWeaponComponent::InitAnimations() 
{
    //�л���������
    auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
    if (EquipFinishedNotify) //ָ��ת���ɹ�������������Ҫ��֪ͨ
    {
        //��ȡί�У����󶨺���
        EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
    }
    else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is forgotten to set"));
        checkNoEntry();//���ʹ�����Ϣ
    }
    //װ������
    for (auto OneWeaponData : WeaponData)
    {
        auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTUReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if (!ReloadFinishedNotify)
        {
            UE_LOG(LogWeaponComponent, Error, TEXT("Reload anim notify is forgotten to set"));
            checkNoEntry(); //���ʹ�����Ϣ
        }
         //��ȡί�У����󶨺���
        ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
    }
}


void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent) {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) return;

    EquipAnimInProgress = false;
}
void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent) {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) return;

    ReloadAnimInProgress = false;
}
bool USTUWeaponComponent::CanFire() const
{
    return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress;
}
bool USTUWeaponComponent::CanEquip() const
{
    return !EquipAnimInProgress && !ReloadAnimInProgress;
}
bool USTUWeaponComponent::CanReload() const
{
    return CurrentWeapon//
        && !EquipAnimInProgress//
        && !ReloadAnimInProgress//
        && CurrentWeapon->CanReload();
}

void USTUWeaponComponent::Reload()
{
    ChangeClip();
}

void USTUWeaponComponent::OnClipEmpty(ASTUBaseWeapon* AmmoEmptyWeapon)
{
    if (!AmmoEmptyWeapon) return;
    if (CurrentWeapon == AmmoEmptyWeapon)
    {
        ChangeClip();
    }
    else
    {
        for (const auto Weapon : Weapons)
        {
            if (Weapon == AmmoEmptyWeapon)
            {
                Weapon->ChangeClip();
            }
        }
    }
}
void USTUWeaponComponent::ChangeClip() 
{
    if (!CanReload()) return;
    CurrentWeapon->StopFire();
    CurrentWeapon->ChangeClip();
    ReloadAnimInProgress = true;
    PlayAnimMontage(CurrentReloadAnimMontage);
}

bool USTUWeaponComponent::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
    if (CurrentWeapon)
    {
        UIData = CurrentWeapon->GetUIData();
        return true;
    }
    return false;
}

bool USTUWeaponComponent::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
    if (CurrentWeapon)
    {
        AmmoData = CurrentWeapon->GetAmmoData();
        return true;
    }
    return false;
}

bool USTUWeaponComponent::TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount)
{
    for (const auto Weapon : Weapons)
    {
        if (Weapon && Weapon->IsA(WeaponType))
        {
            return Weapon->TryToAddAmmo(ClipsAmount);
        }
    }
    return false;
}
bool USTUWeaponComponent::NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType)
{
    for (const auto Weapon : Weapons)
    {
        if (Weapon && Weapon->IsA(WeaponType))
        {
            return !Weapon->IsAmmoFull();
        }
    }
    return false;
}

void USTUWeaponComponent::Zoom(bool Enabled) 
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Zoom(Enabled);
    }
}
