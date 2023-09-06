// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUGameHUD.h"
#include "Engine/Canvas.h"
#include "UI/STUBaseWidget.h"
#include "STUGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUGameHUD, All, All);

//ÿ֡����
void ASTUGameHUD::DrawHUD()
{
    Super::DrawHUD();

    //DrawCrossHair();
}

void ASTUGameHUD::BeginPlay() 
{
    Super::BeginPlay();
    //�����û�����
    //auto PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass);
    //if (PlayerHUDWidget)
    //{
    //    PlayerHUDWidget->AddToViewport();
    //}

    GameWidgets.Add(ESTUMatchState::InProgress, CreateWidget<USTUBaseWidget>(GetWorld(), PlayerHUDWidgetClass));
    GameWidgets.Add(ESTUMatchState::Pause, CreateWidget<USTUBaseWidget>(GetWorld(), PauseWidgetClass));
    GameWidgets.Add(ESTUMatchState::GameOver, CreateWidget<USTUBaseWidget>(GetWorld(), GameOverWidgetClass));

    for (auto GameWidgetPair : GameWidgets)
    {
        const auto GameWidget = GameWidgetPair.Value;
        if (!GameWidget) continue;

        GameWidget->AddToViewport();//��ӵ��ӿڣ��������棩
        GameWidget->SetVisibility(ESlateVisibility::Hidden);//����Ϊ���ɼ�
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            //����ί��
            GameMode->OnMatchStateChanged.AddUObject(this, &ASTUGameHUD::OnMatchStateChanged);
        }
    }
}

void ASTUGameHUD::OnMatchStateChanged(ESTUMatchState State) 
{
    if (CurrentWidge)
    {
        CurrentWidge->SetVisibility(ESlateVisibility::Hidden);
    }
    if (GameWidgets.Contains(State))
    {
        CurrentWidge = GameWidgets[State];
    }
    if (CurrentWidge)
    {
        CurrentWidge->SetVisibility(ESlateVisibility::Visible);
        CurrentWidge->Show();
    }

    UE_LOG(LogSTUGameHUD, Display, TEXT("Match state changed: %s"), *UEnum::GetValueAsString(State));
}

/// <summary>
/// ����׼��
/// </summary>
void ASTUGameHUD::DrawCrossHair() 
{
    //��ȡ��Ļ��С
    //int32 SizeX = Canvas->SizeX;
    //int32 SizeY = Canvas->SizeY;
    const TInterval<float> Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);

    const float HalfLineSize = 10.0f;
    const float LineThickness = 2.0f;
    const FLinearColor LineColor = FLinearColor::Green;
    //�������� ��������Ļ�ϵ���㣨x������Ļ�ϵ���㣨y������Ļ�ϵ���㣨x��;��Ļ�ϵ���㣨y����������ɫ��������ϸ
    //ˮƽ��
    DrawLine(Center.Min - HalfLineSize, Center.Max, Center.Min + HalfLineSize, Center.Max, LineColor, LineThickness);
    //��ֱ��
    DrawLine(Center.Min, Center.Max - HalfLineSize, Center.Min, Center.Max + HalfLineSize, LineColor, LineThickness);
}
