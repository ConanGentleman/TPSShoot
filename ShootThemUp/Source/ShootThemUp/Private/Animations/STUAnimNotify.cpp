// Shoot Them Up Game, All Rights Reserved.


#include "Animations/STUAnimNotify.h"

/// <summary>
/// �����¼�֪ͨ
/// </summary>
/// <param name="MeshComp"></param>
/// <param name="Animation"></param>
void USTUAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}