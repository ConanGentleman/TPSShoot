// Shoot Them Up Game, All Rights Reserved.


#include "AI/EQS/EnvQueryTest_PickupCouldBeTaken.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "Pickups/STUBasePickup.h"

UEnvQueryTest_PickupCouldBeTaken::UEnvQueryTest_PickupCouldBeTaken(const FObjectInitializer& ObjectInitializer) //
    : Super(ObjectInitializer)
{
    Cost = EEnvTestCost::Low;
    ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
    SetWorkOnFloatValues(false);
}
/// <summary>
/// 判断是否能拾取道具（道具隐藏后仍能被查询到）
/// </summary>
/// <param name="QueryInstance"></param>
void UEnvQueryTest_PickupCouldBeTaken::RunTest(FEnvQueryInstance& QueryInstance) const
{
    const auto DataOwner=QueryInstance.Owner.Get();
    BoolValue.BindData(DataOwner,QueryInstance.QueryID);//绑定数据
    const bool WantsBeTakable=BoolValue.GetValue();

    for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)//迭代所有生成实体
    {
        const auto ItemActor =GetItemActor(QueryInstance,It.GetIndex());
        const auto PickupActor=Cast<ASTUBasePickup>(ItemActor);
        if(!PickupActor) continue;

        const auto CouldBeTaken=PickupActor->CouldBeTaken();
        //参数：TestPurpose；FilterType匹配；得分(测试结果）；期望
        It.SetScore(TestPurpose,FilterType,CouldBeTaken,WantsBeTakable); //得分和期望是否匹配
    }
}