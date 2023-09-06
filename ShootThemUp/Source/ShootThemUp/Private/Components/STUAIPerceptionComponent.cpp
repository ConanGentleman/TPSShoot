// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUAIPerceptionComponent.h"
#include "AIController.h"
#include "STUUtils.h"
#include "Components/STUHealthComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

/// <summary>
/// 寻找AI可见范围内的最近敌人
/// </summary>
/// <returns></returns>
AActor* USTUAIPerceptionComponent::GetClosestEnemy() const
{
    TArray<AActor*> PercieveActors;
	//获取能被视野感知到的actor数组 参数：感知类型；存入的actor数组
    GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PercieveActors);
    if (PercieveActors.Num() == 0)
    {
        //获取能被伤害感知到的actor数组
        GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), PercieveActors);
        if (PercieveActors.Num() == 0) return nullptr;
    }

    const auto Controller = Cast<AAIController>(GetOwner());
    if (!Controller) return nullptr;

    const auto Pawn = Controller->GetPawn();
    if (!Pawn) return nullptr;

    float BestDistance = MAX_FLT;
    //最近角色
    AActor* BestPawn = nullptr;
    for (const auto PercieveActor : PercieveActors)
    {
        const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(PercieveActor);

        const auto PercievePawn = Cast<APawn>(PercieveActor);
        const auto AreEnemies = PercievePawn && STUUtils::AreEnemies(Controller, PercievePawn->Controller);

        if (HealthComponent && !HealthComponent->IsDead() && AreEnemies)
        {
            const auto CurrentDistance = (PercieveActor->GetActorLocation() - Pawn->GetActorLocation()).Size();
            if (CurrentDistance < BestDistance)
            {
                BestDistance = CurrentDistance;
                BestPawn = PercieveActor;   
            }
        }
    }
    return BestPawn;
    
}