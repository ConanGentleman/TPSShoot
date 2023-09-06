// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
USTUWeaponFXComponent::USTUWeaponFXComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponFXComponent::PlayImpactFX(const FHitResult& Hit)
{
    auto ImpactData = DefaultImpactData;

    if (Hit.PhysMaterial.IsValid())
    {
        const auto PhysMat = Hit.PhysMaterial.Get();
        if (ImpactDataMap.Contains(PhysMat))
        {
            ImpactData = ImpactDataMap[PhysMat];
        }
    }

    //生成Niagara系统 参数：世界；发射器资产；世界坐标位置；世界方向（法线）;
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), //
        ImpactData.NiagaraEffect,                                            //
        Hit.ImpactPoint,                                       //
        Hit.ImpactNormal.Rotation());

    //decal 生成贴花 参数：世界;贴花材质;贴花大小;贴花在世界坐标的位置;贴花旋转
    auto DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), //
        ImpactData.DecalData.Material,
        ImpactData.DecalData.Size,
        Hit.ImpactPoint, Hit.ImpactNormal.Rotation()
    );

    if (DecalComponent)
    {
        //延迟时间  让贴花消失 参数：消失动画多长时间后开始；淡入淡出动画时间;是否删除所有者贴花（默认true)
        DecalComponent->SetFadeOut(ImpactData.DecalData.LifeTime,ImpactData.DecalData.FadeOutTime);
    }

    //音频
    //在子弹撞击点播放音频
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactData.Sound, Hit.ImpactPoint);
}
