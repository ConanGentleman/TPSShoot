// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "STUNextLocationTask.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API USTUNextLocationTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
    USTUNextLocationTask();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float Radius = 1000.0f;

    //存储黑板键
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FBlackboardKeySelector AimLocationKey;//移动到的目标位置

    //是否把玩家本身作为敌人移动的目标
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool SelfCenter = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI",meta=(EditCondition="!SelfCenter"))
    FBlackboardKeySelector CenterActorKey;
};
