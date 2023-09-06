// Shoot Them Up Game, All Rights Reserved.


#include "AI/STUAIController.h"
#include "AI/STUAICharacter.h"
#include "Components/STUAIPerceptionComponent.h"
#include "Components/STURespawnComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ASTUAIController::ASTUAIController()
{
    STUAIPerceptionComponent = CreateDefaultSubobject<USTUAIPerceptionComponent>("STUAIPerceptionComponent");
    SetPerceptionComponent(*STUAIPerceptionComponent);

    RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("RespawnComponent");

    bWantsPlayerState = true; //让AI生成其PlayerState
}

void ASTUAIController::OnPossess(APawn* InPawn) 
{
    Super::OnPossess(InPawn);

    const auto STUCharacter = Cast<ASTUAICharacter>(InPawn);
    if (STUCharacter)
    {
        RunBehaviorTree(STUCharacter->BehaviorTreeAsset);
    }
}
void ASTUAIController::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
    const auto AimActor = GetFocusOnActor();
    //让当前actor朝向其他actor
    SetFocus(AimActor);
}

AActor* ASTUAIController::GetFocusOnActor() const
{
    if (!GetBlackboardComponent()) return nullptr;
    return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FocusOnKeyName));
}
