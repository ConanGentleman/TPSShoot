// Shoot Them Up Game, All Rights Reserved.


#include "Components/STUCharacterMovementComponent.h"
#include "Player/STUBaseCharacter.h"

float USTUCharacterMovementComponent::GetMaxSpeed() const 
{
    const float MaxSpeed = Super::GetMaxSpeed();
    const ASTUBaseCharacter* Player = Cast<ASTUBaseCharacter>(GetPawnOwner());
    //如果玩家正在奔跑，那么调整最大的速度
    return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}