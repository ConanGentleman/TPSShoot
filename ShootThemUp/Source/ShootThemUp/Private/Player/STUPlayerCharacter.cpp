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

    //弹簧臂组件 用于旋转视角时 更加流畅
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    //将弹簧臂组件连接到角色的根组件
    SpringArmComponent->SetupAttachment(GetRootComponent());
    // pawn控制旋转
    SpringArmComponent->bUsePawnControlRotation = true;
    //设置位置
    SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

    //创建相机组件
    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    //将相机组件连接到弹簧臂组件
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

    //开始发生碰撞体重叠时 调用的函数绑定(委托）  参数：碰撞事件类型；调用函数
    CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionBeginOverlap);
    //结束碰撞体重叠时
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
///检查球星碰撞体是否穿过胶囊 并设置网格可见性
void ASTUPlayerCharacter::CheckCameraOverlap()
{
    //组件是否有重叠
    const auto HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
    //设置网格可见性
    GetMesh()->SetOwnerNoSee(HideMesh);

    TArray<USceneComponent*> MeshChildren;
    //获取网格提的所有子组件 参数：是否是所有层级的后代；返回的子组件数组
    GetMesh()->GetChildrenComponents(true, MeshChildren);

    for (auto MeshChild : MeshChildren)
    {
        const auto MeshChildGeometry = Cast<UPrimitiveComponent>(MeshChild);//强转为UPrimitiveComponent才有SetOwnerNoSee函数
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

    //绑定输入；参数：输入名；调用功能的对象指针；调用函数；
    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUPlayerCharacter::MoveRight);
    //一个参数 直接调用源函数
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUPlayerCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUPlayerCharacter::AddControllerYawInput);
    //绑定跳跃。参数：绑定名称；控件状态;调用功能的对象指针；调用函数；
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUPlayerCharacter::Jump);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUPlayerCharacter::OnStartRunning);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUPlayerCharacter::OnStopRunning);
    //绑定开火
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USTUWeaponComponent::StopFire);
    //切换武器
    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
    //装弹
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);

    //武器开镜瞄准
    DECLARE_DELEGATE_OneParam(FZoomInputSignature, bool);//单播委托(传参)。只能绑定一个函数指针的委托，也就是当执行委托时只能触发一个唯一绑定的函数。
    PlayerInputComponent->BindAction<FZoomInputSignature>("Zoom", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Zoom,true);
    PlayerInputComponent->BindAction<FZoomInputSignature>("Zoom", IE_Released, WeaponComponent, &USTUWeaponComponent::Zoom,false);
}


void ASTUPlayerCharacter::MoveForward(float Amount)
{
    IsMovingForward = Amount > 0.0f;
    if (Amount == 0.0f) return;
    // pawn的函数  用于移动； 参数：移动方向（单位向量）;移动的长度
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

    ////播放死亡（蒙太奇）动画 参数：动画指针
    //// PlayAnimMontage(DeathAnimMontage);

    ////禁止移动
    //GetCharacterMovement()->DisableMovement();

    ////死亡五秒后 销毁
    //SetLifeSpan(LifeSpanOnDeath);

    Super::OnDeath();
    if (Controller)
    { //玩家死亡后 将控制权交给旁观者（Spectater)
        Controller->ChangeState(NAME_Spectating);
    }
    ////玩家死亡 所有碰撞设置为忽略
    //GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    ////停止射击
    //WeaponComponent->StopFire();

    ////播放另一种死亡动画（另一方法）  让角色的骨骼在重力的作用下散架
    //GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ////激活身体的物理模拟
    //GetMesh()->SetSimulatePhysics(true);
}