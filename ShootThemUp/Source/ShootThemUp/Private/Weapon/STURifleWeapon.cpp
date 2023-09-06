// Shoot Them Up Game, All Rights Reserved.


#include "Weapon/STURifleWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

ASTURifleWeapon::ASTURifleWeapon()
{
    WeaponFXComponent=CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTURifleWeapon::BeginPlay()
{
    Super::BeginPlay();

    check(WeaponFXComponent);
}

void ASTURifleWeapon::StartFire()
{
    InitFX(); 
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}
void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetFXActive(false);
}

void ASTURifleWeapon::MakeShot()
{
    if (!GetWorld() || IsAmmoEmpty())
    {
        StopFire();
        return;
    }

    FVector TraceStart, TraceEnd;

    if (!GetTraceData(TraceStart, TraceEnd))
    {
        StopFire();
        return;
    }

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    FVector TraceFXEnd = TraceEnd;
    if (HitResult.bBlockingHit) // true表示发生相交
    {
        TraceFXEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult); //造成伤害

        //绘制直线  参数：世界；线的起始位置；结束位置；线的颜色；线是否一直存在；存在时长；渲染深度；线的粗细
        //DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.0f, 0, 3.0f);

        //DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 5.0f);
        
        //播放特效
        WeaponFXComponent->PlayImpactFX(HitResult);
    }
    SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);
    //else
    //{
    //    DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), TraceEnd, FColor::Red, false, 3.0f, 0, 3.0f);
    //}

    DecreaseAmmo();
}

/// <summary>
/// 获取射线检查信息
/// </summary>
/// <param name="TraceStart"></param>
/// <param name="TraceEnd"></param>
/// <returns></returns>
bool ASTURifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    //获取摄像机位置和方向
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
        return false;

    TraceStart = ViewLocation; // SocketTransform.GetLocation();
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    // VRandCone用于设置随机弹道
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad); // SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

/// <summary>
/// 对敌人造成伤害
/// </summary>
/// <param name="HitResult"></param>
void ASTURifleWeapon::MakeDamage(FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    if (!DamagedActor) return;

    //受伤点事件 会调用OnTakePointDamage委托
    FPointDamageEvent PointDamageEvent;
    PointDamageEvent.HitInfo = HitResult;
    DamagedActor->TakeDamage(DamageAmount, PointDamageEvent, GetController(), this);
}

void ASTURifleWeapon::InitFX() 
{
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }

    if (!FireAudioComponent)
    {
        //将音频在网格的插槽处播放
        FireAudioComponent = UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);
    }
    SetFXActive(true);
}

void ASTURifleWeapon::SetFXActive(bool IsActive)
{
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!IsActive);
        MuzzleFXComponent->SetVisibility(IsActive, true);
    }

    if (FireAudioComponent)
    {
        //音频播放和停止
        IsActive ? FireAudioComponent->Play() : FireAudioComponent->Stop();
    }
}

void ASTURifleWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd) 
{
    //产生子弹射击路径 效果 参数：世界；特效资源；位置
    const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),TraceFX,TraceStart);
    if (TraceFXComponent)
    {
        //设置效果参数  参数：变量名；变量值
        TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
    }
}

AController* ASTURifleWeapon::GetController() const 
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}
/// <summary>
/// 射击开镜
/// </summary>
/// <param name="Enabled"></param>
void ASTURifleWeapon::Zoom(bool Enabled)
{
    const auto Controller = Cast<APlayerController>(GetController());
    if (!Controller || !Controller->PlayerCameraManager) return;

    if (Enabled)
    {
        DefaultCameraFOV = Controller->PlayerCameraManager->GetFOVAngle();
    }

    //设置相机视角
    Controller->PlayerCameraManager->SetFOV(Enabled ? FOVZoomAngle : DefaultCameraFOV);
}