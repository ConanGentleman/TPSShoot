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

	////弹簧臂组件 用于旋转视角时 更加流畅
	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
 //   //将弹簧臂组件连接到角色的根组件
 //   SpringArmComponent->SetupAttachment(GetRootComponent());
	////pawn控制旋转
 //   SpringArmComponent->bUsePawnControlRotation = true;
 //   //设置位置
 //   SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);

	////创建相机组件
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	////将相机组件连接到弹簧臂组件
 //   CameraComponent->SetupAttachment(SpringArmComponent);
    //该组件没有场景、也没有任何表示形式，因此不适用SetupAttachment
    HealthComponent1 = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent1");

    //HealthTextComponent = CreateAbstractDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
    //HealthTextComponent->SetupAttachment(GetRootComponent());
    //HealthTextComponent->SetOwnerNoSee(true);//文本可见性（控制者不能看见自己的）

    //创建武器组件
    WeaponComponent = CreateAbstractDefaultSubobject<USTUWeaponComponent>("WeaponComponent");
}

// Called when the game starts or when spawned
void ASTUBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    check(HealthComponent1);
    check(WeaponComponent);
    //check(HealthTextComponent);
    check(GetCharacterMovement()); //检查组件指针是否为空
    check(GetCapsuleComponent())
    check(GetMesh());


    //由于首先调用actor所有组件的BeginPlay，然后才调用actor本省的BeginPlay，导致第一次的委托广播没有被接收，因此这里显示调用一次函数
    OnHealthChanged(HealthComponent1->GetHealth(),0.0f);
    //绑定死亡委托；  参数:调用者；调用函数
    HealthComponent1->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    //绑定血量变化委托(用于修改血量数值，而不必在Tick修改)；
    HealthComponent1->OnHealthChanged.AddUObject(this, &ASTUBaseCharacter::OnHealthChanged);

    //订阅掉落到地上的委托(自带委托）
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
 
    ////自带的损伤函数(Tick每帧执行掉血);参数：损伤大小；损伤事件（适用于子弹灯）;控制器；造成伤害的actor指针（手榴弹、子弹、小刀）
    //TakeDamage(0.1f,FDamageEvent(),Controller,this);
}

//// Called to bind functionality to input
//void ASTUBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//    check(PlayerInputComponent);
//    check(WeaponComponent);
//
//	//绑定输入；参数：输入名；调用功能的对象指针；调用函数；
//    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUBaseCharacter::MoveForward);
//    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUBaseCharacter::MoveRight);
//	//一个参数 直接调用源函数
//    PlayerInputComponent->BindAxis("LookUp", this, &ASTUBaseCharacter::AddControllerPitchInput);
//    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUBaseCharacter::AddControllerYawInput);
//	//绑定跳跃。参数：绑定名称；控件状态;调用功能的对象指针；调用函数；
//    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUBaseCharacter::Jump);
//    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASTUBaseCharacter::OnStartRunning);
//    PlayerInputComponent->BindAction("Run", IE_Released, this, &ASTUBaseCharacter::OnStopRunning);
//    //绑定开火
//    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::StartFire);
//    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USTUWeaponComponent::StopFire);
//    //切换武器
//    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
//    //装弹
//    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);
//}

//void ASTUBaseCharacter::MoveForward(float Amount) 
//{
//    IsMovingForward = Amount>0.0f;
//    if (Amount == 0.0f)  return;
//	//pawn的函数  用于移动； 参数：移动方向（单位向量）;移动的长度
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
//计算移动方向与forward的夹角情况
float ASTUBaseCharacter::GetMovementDirection() const
{
    if (GetVelocity().IsZero())
        return 0.0f;
    const auto VelocityNormal = GetVelocity().GetSafeNormal();//速度方向向量归一化
    const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
    const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);//叉乘求z
    const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
    return CrossProduct.IsZero() ? Degrees:Degrees * FMath::Sign(CrossProduct.Z) ;
}

void ASTUBaseCharacter::OnDeath() 
{
    UE_LOG(LogBaseCharacter, Display, TEXT("Player %s is dead"), *GetName());

    //播放死亡（蒙太奇）动画 参数：动画指针
    //PlayAnimMontage(DeathAnimMontage);

    //禁止移动
    GetCharacterMovement()->DisableMovement();

    //死亡五秒后 销毁
    SetLifeSpan(LifeSpanOnDeath);

    //if (Controller)
    //{ //玩家死亡后 将控制权交给旁观者（Spectater)
    //    Controller->ChangeState(NAME_Spectating);
    //}
    
    //玩家死亡 所有碰撞设置为忽略
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    //停止射击
    WeaponComponent->StopFire();
    //关闭开镜
    WeaponComponent->Zoom(false);

    //播放另一种死亡动画（另一方法）  让角色的骨骼在重力的作用下散架
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //激活身体的物理模拟
    GetMesh()->SetSimulatePhysics(true);

    //死亡音效 带音量衰减  参数：世界；音频；位置
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound,GetActorLocation());
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit) 
{
    const auto FallVelocityZ = -GetVelocity().Z;

    if (FallVelocityZ<LandedDamageVelocity.X) return;

    //计算坠落伤害 参数：输入变量，输出变量；下路速度
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity,LandedDamage,FallVelocityZ);
    TakeDamage(FinalDamage, FDamageEvent{}, nullptr, nullptr);
}
/// <summary>
/// 设置玩家所属队伍颜色
/// </summary>
/// <param name="Color"></param>
void ASTUBaseCharacter::SetPlayerColor(const FLinearColor& Color) 
{
    //动态创建材料
    const auto MaterialInst = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);

    if (!MaterialInst) return;

    //设置材质  参数：参数名称；颜色本身
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
//	//相机上下角度
//    AddControllerPitchInput(Amount);
//}
//
//void ASTUBaseCharacter::TurnAround(float Amount) 
//{
//    //相机左右角度
//    AddControllerYawInput(Amount);
//}

