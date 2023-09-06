// Shoot Them Up Game, All Rights Reserved.

#include "Weapon/STUProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Components/STUWeaponFXComponent.h"

// Sets default values
ASTUProjectile::ASTUProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    //设置球体半径
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    CollisionComponent->bReturnMaterialOnMove = true;//使物理材质进入最终的HitResult对象（让HitResult带有物理材质信息）
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    //初始速度
    MovementComponent->InitialSpeed = 2000.0f;
    //重力
    MovementComponent->ProjectileGravityScale = 0.0f;
    //特效
    WeaponFXComponent=CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

// Called when the game starts or when spawned
void ASTUProjectile::BeginPlay()
{
    Super::BeginPlay();

    check(MovementComponent);
    check(CollisionComponent);
    check(WeaponFXComponent);

    MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
    //移动忽略哪些actor 参数：actor指针；是否忽略
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(),true);
    //调用碰撞事件委托 参数：当前对象；调用函数
    CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);
    //几秒后删除actor
    SetLifeSpan(LifeSeconds);
}
/// <summary>
/// 碰撞触发事件
/// </summary>
/// <param name="HitComponent"></param>
/// <param name="OtherActor"></param>
/// <param name="OtherComp"></param>
/// <param name="NormalImpulse"></param>
/// <param name="Hit"></param>
void ASTUProjectile::OnProjectileHit(
    UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!GetWorld())
        return;

    //碰撞后停止移动
    MovementComponent->StopMovementImmediately();

    //造成伤害
    //参数：世界；伤害数值；损伤球体的中心；半径；伤害类型；忽略伤害的actor；造成伤害的actor；控制器；伤害半径（ture表示球的任何位置都受到相同伤害，false伤害随中心距离增加而减少）；受伤效果（默认）
    UGameplayStatics::ApplyRadialDamage(GetWorld(), //
        DamageAmount,                               //
        GetActorLocation(),                         //
        DamageRadius,                               //
        UDamageType::StaticClass(),                 //
        {GetOwner()},                               //造成伤害时忽略的actor
        this,                                       //
        GetController(),                              //造成伤害的actor
        DoFullDamage);

    //DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 24, FColor::Red, false, 5.0f);
    WeaponFXComponent->PlayImpactFX(Hit);
    Destroy();
}

AController* ASTUProjectile::GetController() const 
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}