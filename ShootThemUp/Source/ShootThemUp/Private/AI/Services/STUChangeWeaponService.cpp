// Shoot Them Up Game, All Rights Reserved.


#include "AI/Services/STUChangeWeaponService.h"
#include "Components/STUWeaponComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "STUUtils.h"

USTUChangeWeaponService::USTUChangeWeaponService()
{
    NodeName = "Change Weapon";
}

/// <summary>
/// ¸ÅÂÊÇÐ»»ÎäÆ÷
/// </summary>
/// <param name="OwnerComp"></param>
/// <param name="NodeMemory"></param>
/// <param name="DeltaSeconds"></param>
void USTUChangeWeaponService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
    const auto Controller = OwnerComp.GetAIOwner();
    if (Controller)
    {
        const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(Controller->GetPawn());
        if (WeaponComponent && Probability>0 && FMath::FRand() <= Probability)
        {
            WeaponComponent->NextWeapon();
        }
    }

    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}