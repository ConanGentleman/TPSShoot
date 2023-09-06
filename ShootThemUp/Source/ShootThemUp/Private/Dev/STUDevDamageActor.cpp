// Shoot Them Up Game, All Rights Reserved.


#include "Dev/STUDevDamageActor.h"
#include "DrawDebugHelpers.h" //绘制图形的头文件
#include "Kismet/GamePlayStatics.h"

// Sets default values
ASTUDevDamageActor::ASTUDevDamageActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
    SetRootComponent(SceneComponent);
}

// Called when the game starts or when spawned
void ASTUDevDamageActor::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ASTUDevDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//绘制网格球体 参数：世界；球的中心；半径；球的网格数；球的颜色
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius,24,SphereColor);
	//造成伤害 参数：世界；伤害数值；损伤球体的中心；半径；伤害类型；忽略伤害的actor；造成伤害的actor；控制器；伤害半径（ture表示球的任何位置都受到相同伤害，false伤害随中心距离增加而减少）；受伤效果（默认）
    UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), Radius, DamageType, {}, this, nullptr, DoFullDamage);
}

