// Shoot Them Up Game, All Rights Reserved.


#include "Dev/STUDevDamageActor.h"
#include "DrawDebugHelpers.h" //����ͼ�ε�ͷ�ļ�
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
	//������������ ���������磻������ģ��뾶������������������ɫ
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius,24,SphereColor);
	//����˺� ���������磻�˺���ֵ��������������ģ��뾶���˺����ͣ������˺���actor������˺���actor�����������˺��뾶��ture��ʾ����κ�λ�ö��ܵ���ͬ�˺���false�˺������ľ������Ӷ����٣�������Ч����Ĭ�ϣ�
    UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), Radius, DamageType, {}, this, nullptr, DoFullDamage);
}

