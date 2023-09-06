// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

// Sets default values
ASTUBaseWeapon::ASTUBaseWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
}

// Called when the game starts or when spawned
void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();
    check(WeaponMesh);
    checkf(DefaultAmmo.Bullets > 0, TEXT("Bullets count couldn't be less or equal zero"));
    checkf(DefaultAmmo.Clips > 0, TEXT("Clips count couldn't be less or equal zero"));
    CurrentAmmo = DefaultAmmo;
}

void ASTUBaseWeapon::StartFire()
{
    // MakeShot();
    // GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTUBaseWeapon::MakeShot, TimeBetweenShots,true);
}
void ASTUBaseWeapon::StopFire()
{
    // GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void ASTUBaseWeapon::MakeShot()
{
    // if (!GetWorld()) return;

    // FVector TraceStart, TraceEnd;

    // if (!GetTraceData(TraceStart, TraceEnd))  return;

    // FHitResult HitResult;
    // MakeHit(HitResult, TraceStart, TraceEnd);

    // if (HitResult.bBlockingHit) //true��ʾ�����ཻ
    //{
    //    MakeDamage(HitResult); //����˺�

    //    //����ֱ��  ���������磻�ߵ���ʼλ�ã�����λ�ã��ߵ���ɫ�����Ƿ�һֱ���ڣ�����ʱ������Ⱦ��ȣ��ߵĴ�ϸ
    //    DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.0f, 0, 3.0f);

    //    DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 5.0f);
    //
    //}
    // else
    //{
    //    DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), TraceEnd, FColor::Red, false, 3.0f, 0, 3.0f);
    //}
}

//APlayerController* ASTUBaseWeapon::GetPlayerController() const
//{
//
//    const auto Player = Cast<ACharacter>(GetOwner());
//    if (!Player)
//        return nullptr;
//
//    return Player->GetController<APlayerController>();
//}
/// <summary>
/// ��ȡ�������Ϣ
/// </summary>
/// <param name="ViewLocation"></param>
/// <param name="ViewRotation"></param>
/// <returns></returns>
bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto STUCharacter = Cast<ACharacter>(GetOwner());
    if (!STUCharacter) return false;
    //�ж�����ҿ��Ƶ�����
    if (STUCharacter->IsPlayerControlled())
    {
        const auto Controller = STUCharacter->GetController<APlayerController>();
        if (!Controller) return false;

        //��ȡ�����λ�úͷ���
        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else//�����AI ��ֻ�賯ǹ�ڷ������
    {
        ViewLocation = GetMuzzleWorldLocation();
        ViewRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
    }

    return true;
}
/// <summary>
/// ��ȡ���������Ϣ
/// </summary>
/// <returns></returns>
FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
    //��۵�transform��Ϣ
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}
/// <summary>
/// ��ȡ���߼����Ϣ
/// </summary>
/// <param name="TraceStart"></param>
/// <param name="TraceEnd"></param>
/// <returns></returns>
bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    //��ȡ�����λ�úͷ���
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
        return false;

    TraceStart = ViewLocation; // SocketTransform.GetLocation();
    // const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    // VRandCone���������������
    const FVector ShootDirection = ViewRotation.Vector(); // SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}
/// <summary>
/// ���߼��
/// </summary>
/// <param name="HitResult"></param>
/// <param name="TraceStart"></param>
/// <param name="TraceEnd"></param>
void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
    if (!GetWorld())
        return;
    //���������ߵ�һ���ཻ�������Ϣ  ���������������ߵ�һ���ཻ�������Ϣ;�߶���㣻�߶��յ�;��ײͨ��
    //����ֵ: �ཻ����true
    FCollisionQueryParams CollisionParams;
    //���Ŀ�� �����Լ�
    CollisionParams.AddIgnoredActor(GetOwner());
    //ʹ������ʽ������յ�HitResult������HitResult�������������Ϣ��
    CollisionParams.bReturnPhysicalMaterial = true;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}
///// <summary>
///// �Ե�������˺�
///// </summary>
///// <param name="HitResult"></param>
// void ASTUBaseWeapon::MakeDamage(FHitResult& HitResult)
//{
//    const auto DamagedActor = HitResult.GetActor();
//    if (!DamagedActor) return;
//
//    DamagedActor->TakeDamage(DamageAmount, FDamageEvent(), GetPlayerController(), this);
//}

/// <summary>
/// ���ٵ�ϻ
/// </summary>
void ASTUBaseWeapon::DecreaseAmmo()
{
    if (CurrentAmmo.Bullets == 0)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("```Clip is empty`````"));
        return;
    }

    CurrentAmmo.Bullets--;

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        StopFire();
        OnClipEmpty.Broadcast(this);
    }
}
/// <summary>
/// ���õ�ϻ�Ƿ���
/// </summary>
/// <returns></returns>
bool ASTUBaseWeapon::IsAmmoEmpty() const
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}
/// <summary>
/// �ӵ��Ƿ���
/// </summary>
/// <returns></returns>
bool ASTUBaseWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

void ASTUBaseWeapon::ChangeClip()
{
    if (!CurrentAmmo.Infinite)
    {
        if (CurrentAmmo.Clips == 0)
        {
            UE_LOG(LogBaseWeapon, Warning, TEXT("```No more Clips`````"));
            return;
        }
        CurrentAmmo.Clips--;
    }
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;
    UE_LOG(LogBaseWeapon, Display, TEXT("````Change Clips`````"));
}
bool ASTUBaseWeapon::CanReload() const
{
    return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
    ;
}
void ASTUBaseWeapon::LogAmmo()
{
    FString AmmoInfo = "Ammo:" + FString::FromInt(CurrentAmmo.Bullets) + "/";
    AmmoInfo += CurrentAmmo.Infinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
    UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);
}

bool ASTUBaseWeapon::IsAmmoFull() const
{
    return CurrentAmmo.Clips == DefaultAmmo.Clips && //
           CurrentAmmo.Bullets == DefaultAmmo.Bullets;
}
/// <summary>
/// ʰȡ���� ��ӵ���
/// </summary>
/// <param name="ClipAmount"></param>
/// <returns></returns>
bool ASTUBaseWeapon::TryToAddAmmo(int32 ClipAmount)
{
    if (CurrentAmmo.Infinite || IsAmmoFull() || ClipAmount <= 0)
        return false;

    if (IsAmmoEmpty())
    {
        UE_LOG(LogBaseWeapon, Display, TEXT("Ammo was empty"));
        CurrentAmmo.Clips = FMath::Clamp(ClipAmount, 0, DefaultAmmo.Clips + 1);
        OnClipEmpty.Broadcast(this); //����
    }
    else if (CurrentAmmo.Clips < DefaultAmmo.Clips)
    {
        const auto NextClipsAmount = CurrentAmmo.Clips + ClipAmount;
        if (DefaultAmmo.Clips - NextClipsAmount >= 0)
        {
            CurrentAmmo.Clips = NextClipsAmount;
            UE_LOG(LogBaseWeapon, Display, TEXT("Clips were added"));
        }
        else
        {
            CurrentAmmo.Clips = DefaultAmmo.Clips;
            CurrentAmmo.Bullets = DefaultAmmo.Bullets;
            UE_LOG(LogBaseWeapon, Display, TEXT("Ammo is full"));
        }
    }
    else
    {
        CurrentAmmo.Bullets = DefaultAmmo.Bullets;
        UE_LOG(LogBaseWeapon, Display, TEXT("Bullets were added"));
    }
    return true;
}

UNiagaraComponent* ASTUBaseWeapon::SpawnMuzzleFX()
{
    //�������Ч������Ч�� ������ָ��Niagaraϵͳ��ָ�룻Niagaraϵͳ���ӵ��������������ƣ�λ�ã���ת��ö���ࣻ�Ƿ��Զ�����
    return UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFX,
                                                 WeaponMesh,
                                                 MuzzleSocketName,
                                                 FVector::ZeroVector,
                                                 FRotator::ZeroRotator,
                                                 EAttachLocation::SnapToTarget,
                                                 true);
}