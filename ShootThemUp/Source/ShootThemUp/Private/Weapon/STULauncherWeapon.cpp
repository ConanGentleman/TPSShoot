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
        //������Ƶ ��PlaySoundAtLocation���ƣ���������ӵ��AudioComponent����ֵ
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());
        return;
    }

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))  return;

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    //�񵯷���
    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    //��ǹ������λ���ӳ������񵯣� UGameplayStatics::BeginDeferredActorSpawnFromClass�� ���������磻���ɵ��ࣻ���ɵ����Transform����  
    //ֱ�����ɣ�GetWorld()->SpawnActorDeferred��
    ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    { 
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(GetOwner());//��ӵ����
        //�������  ����BeginPlay ʹ��FinishSpawning
        Projectile->FinishSpawning(SpawnTransform);
    }

    DecreaseAmmo();
    //����Ч��
    SpawnMuzzleFX();
    //���ſ�����Ƶ ����������ӵ����������,�Ա����λ�ü�������������  ��������Ƶ��������ӵ��������۵����֣�
    UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);
} 