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
    //��������뾶
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
    //ÿ֡��תһ��
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
/// �������
/// </summary>
void ASTUBasePickup::PickupWasTaken()
{
    //�ر���ײ
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    if (GetRootComponent())
    {
        //����Ϊ���ɼ� ��������ǰ����Ƿ���Ⱦ���Ƿ񽫿ɼ���Ӧ�������������
        GetRootComponent()->SetVisibility(false, true);
    }

    //������ʱ��   ��������
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASTUBasePickup::Respawn, RespawnTime);

    //������Ч
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupTakenSound, GetActorLocation());
}
/// <summary>
/// �������ɵ���
/// </summary>
void ASTUBasePickup::Respawn()
{
    GenerateRotationYaw();
    //������ײ
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    if (GetRootComponent())
    {
        //����Ϊ���ɼ� ��������ǰ����Ƿ���Ⱦ(�ɼ�)���Ƿ񽫿ɼ���Ӧ�������������
        GetRootComponent()->SetVisibility(true, true);
    }
}
/// <summary>
/// ��ʰȡ����ת
/// </summary>
void ASTUBasePickup::GenerateRotationYaw() 
{
    const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
    RotationYaw = FMath::RandRange(1.0f, 2.0f) * Direction;//�������
}
/// <summary>
/// �Ƿ���ʰȡ
/// </summary>
/// <returns></returns>
bool ASTUBasePickup::CouldBeTaken() const
{
    //�������ʹ�ü�ʱ�� ��Ϊ����ʰȡ
    return !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle);
}