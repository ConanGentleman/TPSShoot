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

    // if (HitResult.bBlockingHit) //true表示发生相交
    //{
    //    MakeDamage(HitResult); //造成伤害

    //    //绘制直线  参数：世界；线的起始位置；结束位置；线的颜色；线是否一直存在；存在时长；渲染深度；线的粗细
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
/// 获取摄像机信息
/// </summary>
/// <param name="ViewLocation"></param>
/// <param name="ViewRotation"></param>
/// <returns></returns>
bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto STUCharacter = Cast<ACharacter>(GetOwner());
    if (!STUCharacter) return false;
    //判断是玩家控制的输入
    if (STUCharacter->IsPlayerControlled())
    {
        const auto Controller = STUCharacter->GetController<APlayerController>();
        if (!Controller) return false;

        //获取摄像机位置和方向
        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else//如果是AI 则只需朝枪口方向射击
    {
        ViewLocation = GetMuzzleWorldLocation();
        ViewRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
    }

    return true;
}
/// <summary>
/// 获取武器插槽信息
/// </summary>
/// <returns></returns>
FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
    //插槽的transform信息
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}
/// <summary>
/// 获取射线检查信息
/// </summary>
/// <param name="TraceStart"></param>
/// <param name="TraceEnd"></param>
/// <returns></returns>
bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    //获取摄像机位置和方向
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
        return false;

    TraceStart = ViewLocation; // SocketTransform.GetLocation();
    // const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    // VRandCone用于设置随机弹道
    const FVector ShootDirection = ViewRotation.Vector(); // SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}
/// <summary>
/// 射线检测
/// </summary>
/// <param name="HitResult"></param>
/// <param name="TraceStart"></param>
/// <param name="TraceEnd"></param>
void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
    if (!GetWorld())
        return;
    //反射与射线第一个相交对象的信息  参数：反射与射线第一个相交对象的信息;线段起点；线段终点;碰撞通道
    //返回值: 相交返回true
    FCollisionQueryParams CollisionParams;
    //射击目标 忽略自己
    CollisionParams.AddIgnoredActor(GetOwner());
    //使物理材质进入最终的HitResult对象（让HitResult带有物理材质信息）
    CollisionParams.bReturnPhysicalMaterial = true;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}
///// <summary>
///// 对敌人造成伤害
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
/// 减少弹匣
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
/// 备用弹匣是否还有
/// </summary>
/// <returns></returns>
bool ASTUBaseWeapon::IsAmmoEmpty() const
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}
/// <summary>
/// 子弹是否打空
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
/// 拾取弹夹 添加弹夹
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
        OnClipEmpty.Broadcast(this); //换弹
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
    //产生冲击效果（特效） 参数：指向Niagara系统的指针；Niagara系统连接到的组件；插槽名称；位置；旋转；枚举类；是否自动销毁
    return UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFX,
                                                 WeaponMesh,
                                                 MuzzleSocketName,
                                                 FVector::ZeroVector,
                                                 FRotator::ZeroRotator,
                                                 EAttachLocation::SnapToTarget,
                                                 true);
}