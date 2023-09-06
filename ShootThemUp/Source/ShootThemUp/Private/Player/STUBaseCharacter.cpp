// Shoot Them Up Game, All Rights Reserved.


#include "Player/STUBaseCharacter.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, All, All);

// Sets default values
ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	////���ɱ���� ������ת�ӽ�ʱ ��������
	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
 //   //�����ɱ�������ӵ���ɫ�ĸ����
 //   SpringArmComponent->SetupAttachment(GetRootComponent());
	////pawn������ת
 //   SpringArmComponent->bUsePawnControlRotation = true;
 //   //����λ��
 //   SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

	////����������
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	////�����������ӵ����ɱ����
 //   CameraComponent->SetupAttachment(SpringArmComponent);
    //�����û�г�����Ҳû���κα�ʾ��ʽ����˲�����SetupAttachment
    HealthComponent1 = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent1");

    //HealthTextComponent = CreateAbstractDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
    //HealthTextComponent->SetupAttachment(GetRootComponent());
    //HealthTextComponent->SetOwnerNoSee(true);//�ı��ɼ��ԣ������߲��ܿ����Լ��ģ�

    //�����������
    WeaponComponent = CreateAbstractDefaultSubobject<USTUWeaponComponent>("WeaponComponent");
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    check(HealthComponent1);
    check(WeaponComponent);
    //check(HealthTextComponent);
    check(GetCharacterMovement()); //������ָ���Ƿ�Ϊ��
    check(GetCapsuleComponent())
    check(GetMesh());


    //�������ȵ���actor���������BeginPlay��Ȼ��ŵ���actor��ʡ��BeginPlay�����µ�һ�ε�ί�й㲥û�б����գ����������ʾ����һ�κ���
    OnHealthChanged(HealthComponent1->GetHealth(),0.0f);
    //������ί�У�  ����:�����ߣ����ú���
    HealthComponent1->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    //��Ѫ���仯ί��(�����޸�Ѫ����ֵ����������Tick�޸�)��
    HealthComponent1->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    //���ĵ��䵽���ϵ�ί��(�Դ�ί�У�
    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);

}
void ASTUBaseCharacter::OnHealthChanged(float Health, float HealthDelta)
{
    // UE_LOG(LogBaseCharacter, Error, TEXT("HealthText: %f"), Health);
    // HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

// Called every frame
void ASTUBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
    ////�Դ������˺���(Tickÿִ֡�е�Ѫ);���������˴�С�������¼����������ӵ��ƣ�;������������˺���actorָ�루���񵯡��ӵ���С����
    //TakeDamage(0.1f,FDamageEvent(),Controller,this);
}

//// Called to bind functionality to input
//void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//    check(PlayerInputComponent);
//    check(WeaponComponent);
//
//	//�����룻�����������������ù��ܵĶ���ָ�룻���ú�����
//    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUBaseCharacter::MoveForward);
//    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUBaseCharacter::MoveRight);
//	//һ������ ֱ�ӵ���Դ����
//    PlayerInputComponent->BindAxis("LookUp", this, &ASTUBaseCharacter::AddControllerPitchInput);
//    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUBaseCharacter::AddControllerYawInput);
//	//����Ծ�������������ƣ��ؼ�״̬;���ù��ܵĶ���ָ�룻���ú�����
//    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUBaseCharacter::Jump);
//    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUBaseCharacter::OnStartRunning);
//    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUBaseCharacter::OnStopRunning);
//    //�󶨿���
//    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::StartFire);
//    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USTUWeaponComponent::StopFire);
//    //�л�����
//    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
//    //װ��
//    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);
//}

//void ASTUBaseCharacter::MoveForward(float Amount) 
//{
//    IsMovingForward = Amount>0.0f;
//    if (Amount == 0.0f)  return;
//	//pawn�ĺ���  �����ƶ��� �������ƶ����򣨵�λ������;�ƶ��ĳ���
//    AddMovementInput(GetActorForwardVector(), Amount);
//}
//
//void ASTUBaseCharacter::MoveRight(float Amount) 
//{
//    if (Amount == 0.0f)  return;
//    AddMovementInput(GetActorRightVector(), Amount);
//}
//
//void ASTUBaseCharacter::OnStartRunning() 
//{
//    WantsToRun = true;
//}
//
//void ASTUBaseCharacter::OnStopRunning() 
//{
//    WantsToRun = false;
//}

bool ASTUBaseCharacter::IsRunning() const
{
    //return WantsToRun && IsMovingForward && !GetVelocity().IsZero();
    return false;
}
//�����ƶ�������forward�ļн����
float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero())
        return 0.0f;
    const auto VelocityNormal = GetVelocity().GetSafeNormal();//�ٶȷ���������һ��
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);//�����z
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    return CrossProduct.IsZero() ? Degrees:Degrees * FMath::Sign(CrossProduct.Z) ;
}

void ASTUBaseCharacter::OnDeath() 
{
    UE_LOG(LogBaseCharacter, Display, TEXT("Player %s is dead"), *GetName());

    //������������̫�棩���� ����������ָ��
    //PlayAnimMontage(DeathAnimMontage);

    //��ֹ�ƶ�
    GetCharacterMovement()->DisableMovement();

    //��������� ����
    SetLifeSpan(LifeSpanOnDeath);

    //if (Controller)
    //{ //��������� ������Ȩ�����Թ��ߣ�Spectater)
    //    Controller->ChangeState(NAME_Spectating);
    //}
    
    //������� ������ײ����Ϊ����
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    //ֹͣ���
    WeaponComponent->StopFire();
    //�رտ���
    WeaponComponent->Zoom(false);

    //������һ��������������һ������  �ý�ɫ�Ĺ�����������������ɢ��
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //�������������ģ��
    GetMesh()->SetSimulatePhysics(true);

    //������Ч ������˥��  ���������磻��Ƶ��λ��
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound,GetActorLocation());
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit) 
{
    const auto FallVelocityZ = -GetVelocity().Z;

    if (FallVelocityZ<LandedDamageVelocity.X) return;

    //����׹���˺� ��������������������������·�ٶ�
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity,LandedDamage,FallVelocityZ);
    TakeDamage(FinalDamage, FDamageEvent{}, nullptr, nullptr);
}
/// <summary>
/// �����������������ɫ
/// </summary>
/// <param name="Color"></param>
void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color) 
{
    //��̬��������
    const auto MaterialInst = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);

    if (!MaterialInst) return;

    //���ò���  �������������ƣ���ɫ����
    MaterialInst->SetVectorParameterValue(MaterialColorName,Color);
}

void ASTUBaseCharacter::TurnOff() 
{
    WeaponComponent->StopFire();
    WeaponComponent->Zoom(false);
    Super::TurnOff();
}
void ASTUBaseCharacter::Reset() 
{
    WeaponComponent->StopFire();
    WeaponComponent->Zoom(false);
    Super::Reset();
}

//void ASTUBaseCharacter::LookUp(float Amount) 
//{
//	//������½Ƕ�
//    AddControllerPitchInput(Amount);
//}
//
//void ASTUBaseCharacter::TurnAround(float Amount) 
//{
//    //������ҽǶ�
//    AddControllerYawInput(Amount);
//}

