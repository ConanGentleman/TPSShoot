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
    //��������뾶
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    CollisionComponent->bReturnMaterialOnMove = true;//ʹ������ʽ������յ�HitResult������HitResult�������������Ϣ��
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    //��ʼ�ٶ�
    MovementComponent->InitialSpeed = 2000.0f;
    //����
    MovementComponent->ProjectileGravityScale = 0.0f;
    //��Ч
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
    //�ƶ�������Щactor ������actorָ�룻�Ƿ����
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(),true);
    //������ײ�¼�ί�� ��������ǰ���󣻵��ú���
    CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);
    //�����ɾ��actor
    SetLifeSpan(LifeSeconds);
}
/// <summary>
/// ��ײ�����¼�
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

    //��ײ��ֹͣ�ƶ�
    MovementComponent->StopMovementImmediately();

    //����˺�
    //���������磻�˺���ֵ��������������ģ��뾶���˺����ͣ������˺���actor������˺���actor�����������˺��뾶��ture��ʾ����κ�λ�ö��ܵ���ͬ�˺���false�˺������ľ������Ӷ����٣�������Ч����Ĭ�ϣ�
    UGameplayStatics::ApplyRadialDamage(GetWorld(), //
        DamageAmount,                               //
        GetActorLocation(),                         //
        DamageRadius,                               //
        UDamageType::StaticClass(),                 //
        {GetOwner()},                               //����˺�ʱ���Ե�actor
        this,                                       //
        GetController(),                              //����˺���actor
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