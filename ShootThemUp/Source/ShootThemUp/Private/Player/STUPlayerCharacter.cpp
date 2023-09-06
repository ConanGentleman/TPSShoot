// Shoot Them Up Game, All Rights Reserved.


#include "Player/STUPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


ASTUPlayerCharacter::ASTUPlayerCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //���ɱ���� ������ת�ӽ�ʱ ��������
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    //�����ɱ�������ӵ���ɫ�ĸ����
    SpringArmComponent->SetupAttachment(GetRootComponent());
    // pawn������ת
    SpringArmComponent->bUsePawnControlRotation = true;
    //����λ��
    SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

    //����������
    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    //�����������ӵ����ɱ����
    CameraComponent->SetupAttachment(SpringArmComponent);

    CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>("CameraCollisionComponent");
    CameraCollisionComponent->SetupAttachment(CameraComponent);
    CameraCollisionComponent->SetSphereRadius(10.0f);
    CameraCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}
void ASTUPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(CameraCollisionComponent);

    //��ʼ������ײ���ص�ʱ ���õĺ�����(ί�У�  ��������ײ�¼����ͣ����ú���
    CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionBeginOverlap);
    //������ײ���ص�ʱ
    CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionEndOverlap);
}

void ASTUPlayerCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    CheckCameraOverlap();
}

void ASTUPlayerCharacter::OnCameraCollisionEndOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    CheckCameraOverlap();
}
///���������ײ���Ƿ񴩹����� ����������ɼ���
void ASTUPlayerCharacter::CheckCameraOverlap()
{
    //����Ƿ����ص�
    const auto HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
    //��������ɼ���
    GetMesh()->SetOwnerNoSee(HideMesh);

    TArray<USceneComponent*> MeshChildren;
    //��ȡ���������������� �������Ƿ������в㼶�ĺ�������ص����������
    GetMesh()->GetChildrenComponents(true, MeshChildren);

    for (auto MeshChild : MeshChildren)
    {
        const auto MeshChildGeometry = Cast<UPrimitiveComponent>(MeshChild);//ǿתΪUPrimitiveComponent����SetOwnerNoSee����
        if (MeshChildGeometry)
        {
            MeshChildGeometry->SetOwnerNoSee(HideMesh);
        }
    }
}

// Called to bind functionality to input
void ASTUPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    check(WeaponComponent);

    //�����룻�����������������ù��ܵĶ���ָ�룻���ú�����
    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUPlayerCharacter::MoveRight);
    //һ������ ֱ�ӵ���Դ����
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUPlayerCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUPlayerCharacter::AddControllerYawInput);
    //����Ծ�������������ƣ��ؼ�״̬;���ù��ܵĶ���ָ�룻���ú�����
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUPlayerCharacter::Jump);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUPlayerCharacter::OnStartRunning);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUPlayerCharacter::OnStopRunning);
    //�󶨿���
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USTUWeaponComponent::StopFire);
    //�л�����
    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
    //װ��
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);

    //����������׼
    DECLARE_DELEGATE_OneParam(FZoomInputSignature, bool);//����ί��(����)��ֻ�ܰ�һ������ָ���ί�У�Ҳ���ǵ�ִ��ί��ʱֻ�ܴ���һ��Ψһ�󶨵ĺ�����
    PlayerInputComponent->BindAction<FZoomInputSignature>("Zoom", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Zoom,true);
    PlayerInputComponent->BindAction<FZoomInputSignature>("Zoom", IE_Released, WeaponComponent, &USTUWeaponComponent::Zoom,false);
}


void ASTUPlayerCharacter::MoveForward(float Amount)
{
    IsMovingForward = Amount > 0.0f;
    if (Amount == 0.0f) return;
    // pawn�ĺ���  �����ƶ��� �������ƶ����򣨵�λ������;�ƶ��ĳ���
    AddMovementInput(GetActorForwardVector(), Amount);
}

void ASTUPlayerCharacter::MoveRight(float Amount)
{
    if (Amount == 0.0f) return;
    AddMovementInput(GetActorRightVector(), Amount);
}

void ASTUPlayerCharacter::OnStartRunning()
{
    WantsToRun = true;
}

void ASTUPlayerCharacter::OnStopRunning()
{
    WantsToRun = false;
}

bool ASTUPlayerCharacter::IsRunning() const
{
    return WantsToRun && IsMovingForward && !GetVelocity().IsZero();
}

void ASTUPlayerCharacter::OnDeath()
{
    //UE_LOG(LogBaseCharacter, Display, TEXT("Player %s is dead"), *GetName());

    ////������������̫�棩���� ����������ָ��
    //// PlayAnimMontage(DeathAnimMontage);

    ////��ֹ�ƶ�
    //GetCharacterMovement()->DisableMovement();

    ////��������� ����
    //SetLifeSpan(LifeSpanOnDeath);

    Super::OnDeath();
    if (Controller)
    { //��������� ������Ȩ�����Թ��ߣ�Spectater)
        Controller->ChangeState(NAME_Spectating);
    }
    ////������� ������ײ����Ϊ����
    //GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    ////ֹͣ���
    //WeaponComponent->StopFire();

    ////������һ��������������һ������  �ý�ɫ�Ĺ�����������������ɢ��
    //GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ////�������������ģ��
    //GetMesh()->SetSimulatePhysics(true);
}