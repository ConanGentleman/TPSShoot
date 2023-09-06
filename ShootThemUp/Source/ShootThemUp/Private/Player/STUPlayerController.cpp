// Shoot Them Up Game, All Rights Reserved.


#include "Player/STUPlayerController.h"
#include "Components/STURespawnComponent.h"
#include "STUGameModeBase.h"
#include "STUGameInstance.h"

ASTUPlayerController::ASTUPlayerController()
{
    RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("RespawnComponent");
}

void ASTUPlayerController::BeginPlay() 
{
    Super::BeginPlay();
    if (GetWorld())
    {
        
        if (const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode()))
        {
            //����ί��
            GameMode->OnMatchStateChanged.AddUObject(this, &ASTUPlayerController::OnMatchStateChanged);
        }
    }
}
/// <summary>
/// ���ù����ʾ
/// </summary>
/// <param name="State"></param>
void ASTUPlayerController::OnMatchStateChanged(ESTUMatchState State)
{
    if (State == ESTUMatchState::InProgress)
    {
        SetInputMode(FInputModeGameOnly());//���ù��ģʽ������UI����ʱ�����һ����Ϸ�ڲ��ܽ��н��������⣩
        //�رչ��
        bShowMouseCursor = false;
    }
    else
    {
        SetInputMode(FInputModeUIOnly());//���ù��ģʽ
        //�������
        bShowMouseCursor = true;
    }
}
void ASTUPlayerController::OnPossess(APawn* InPawn) 
{
    Super::OnPossess(InPawn);

    OnNewPawn.Broadcast(InPawn);
}

void ASTUPlayerController::SetupInputComponent() 
{
    Super::SetupInputComponent();
    if(!InputComponent) return;

    InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASTUPlayerController::OnPauseGame);
    InputComponent->BindAction("Mute", IE_Pressed, this, &ASTUPlayerController::OnMuteSound);
}

void ASTUPlayerController::OnPauseGame() 
{
    if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;

    //��ͣ��Ϸ  ��������ͣ��Ϸ�Ŀ��������Ƿ���ͣ��ʹ��Ĭ��ֵ��
    GetWorld()->GetAuthGameMode()->SetPause(this);
}

void ASTUPlayerController::OnMuteSound()
{
    if (!GetWorld()) return;
    const auto STUGameInstance = GetWorld() -> GetGameInstance<USTUGameInstance>();
    if (!STUGameInstance) return;

    STUGameInstance->ToggleVolume();
}
