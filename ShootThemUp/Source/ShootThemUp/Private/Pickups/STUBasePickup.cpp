// Shoot Them Up Game, All Rights Reserved.


#include "Pickups/STUBasePickup.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePickup, All, All);

// Sets default values
ASTUBasePickup::ASTUBasePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    //设置球体半径
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SetRootComponent(CollisionComponent);

}

// Called when the game starts or when spawned
void ASTUBasePickup::BeginPlay()
{
	Super::BeginPlay();
	
    check(CollisionComponent);

    GenerateRotationYaw();
}

// Called every frame
void ASTUBasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    //每帧旋转一点
    AddActorLocalRotation(FRotator(0.0f, RotationYaw ,0.0f));
}

void ASTUBasePickup::NotifyActorBeginOverlap(AActor* OtherActor) 
{
    Super::NotifyActorBeginOverlap(OtherActor);

    const auto Pawn = Cast<APawn>(OtherActor);
    if (GivePickUpTo(Pawn))
    {
        PickupWasTaken();
    }

}

bool ASTUBasePickup::GivePickUpTo(APawn* PlayerPawn)
{
    return false;
}
/// <summary>
/// 捡起道具
/// </summary>
void ASTUBasePickup::PickupWasTaken()
{
    //关闭碰撞
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    if (GetRootComponent())
    {
        //设置为不可见 参数：当前组件是否被渲染；是否将可见性应用于所有子组件
        GetRootComponent()->SetVisibility(false, true);
    }

    //重生计时器   道具重现
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASTUBasePickup::Respawn, RespawnTime);

    //播放音效
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupTakenSound, GetActorLocation());
}
/// <summary>
/// 重新生成道具
/// </summary>
void ASTUBasePickup::Respawn()
{
    GenerateRotationYaw();
    //开启碰撞
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    if (GetRootComponent())
    {
        //设置为不可见 参数：当前组件是否被渲染(可见)；是否将可见性应用于所有子组件
        GetRootComponent()->SetVisibility(true, true);
    }
}
/// <summary>
/// 让拾取物旋转
/// </summary>
void ASTUBasePickup::GenerateRotationYaw() 
{
    const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
    RotationYaw = FMath::RandRange(1.0f, 2.0f) * Direction;//随机生成
}
/// <summary>
/// 是否能拾取
/// </summary>
/// <returns></returns>
bool ASTUBasePickup::CouldBeTaken() const
{
    //如果正在使用计时器 则为可以拾取
    return !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle);
}