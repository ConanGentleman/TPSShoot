// Shoot Them Up Game, All Rights Reserved.


#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void ASTULauncherWeapon::StartFire()
{
    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!GetWorld()) return;

    if (IsAmmoEmpty())
    {
        //播放音频 与PlaySoundAtLocation类似，区别在与拥有AudioComponent返回值
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());
        return;
    }

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))  return;

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    //榴弹方向
    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    //在枪口所在位置延迟生成榴弹（ UGameplayStatics::BeginDeferredActorSpawnFromClass） 参数：世界；生成的类；生成的类的Transform参数  
    //直接生成（GetWorld()->SpawnActorDeferred）
    ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    { 
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(GetOwner());//榴弹拥有者
        //完成生成  调用BeginPlay 使用FinishSpawning
        Projectile->FinishSpawning(SpawnTransform);
    }

    DecreaseAmmo();
    //开火效果
    SpawnMuzzleFX();
    //播放开火音频 （将声音添加到网格骨骼上,以便基于位置计算声音参数）  参数：音频；声音添加的组件；插槽的名字；
    UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);
} 