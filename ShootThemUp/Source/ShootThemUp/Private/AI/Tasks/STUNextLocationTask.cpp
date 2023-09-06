// Shoot Them Up Game, All Rights Reserved.


#include "AI/Tasks/STUNextLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

USTUNextLocationTask::USTUNextLocationTask()
{
    //任务名称
    NodeName = "Next Location";
}

EBTNodeResult::Type USTUNextLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const auto Controller = OwnerComp.GetAIOwner();
    const auto Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Controller || !Blackboard) return EBTNodeResult::Failed;

    const auto Pawn = Controller->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    //获取导航系统
    const auto NavSys = UNavigationSystemV1::GetCurrent(Pawn);
    if (!NavSys) return EBTNodeResult::Failed;

    FNavLocation NavLocation;
    auto Location = Pawn->GetActorLocation();
    //是否以CenterActor为中心周围生成随机点 （在目标点周围来回移动）
    if (!SelfCenter)
    {
        auto CenterActor = Cast<AActor>(Blackboard->GetValueAsObject(CenterActorKey.SelectedKeyName));
        if (!CenterActor) return EBTNodeResult::Failed;
        Location = CenterActor->GetActorLocation();
    }

    //获取随机点（可达的） 参数：中心点位置；搜索半径；返回找到的点
    const auto Found = NavSys->GetRandomReachablePointInRadius(Location, Radius, NavLocation);
    if (!Found) return EBTNodeResult::Failed;

    //设置黑板值   参数：变量名；变量值
    Blackboard->SetValueAsVector(AimLocationKey.SelectedKeyName, NavLocation.Location);

    return EBTNodeResult::Succeeded;
}
