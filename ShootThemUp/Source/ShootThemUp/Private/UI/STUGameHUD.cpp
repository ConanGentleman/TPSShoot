// Shoot Them Up Game, All Rights Reserved.


#include "UI/STUGameHUD.h"
#include "Engine/Canvas.h"
#include "UI/STUBaseWidget.h"
#include "STUGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUGameHUD, All, All);

//每帧调用
void ASTUGameHUD::DrawHUD()
{
    Super::DrawHUD();

    //DrawCrossHair();
}

void ASTUGameHUD::BeginPlay() 
{
    Super::BeginPlay();
    //创建用户界面
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

        GameWidget->AddToViewport();//添加到视口（创建界面）
        GameWidget->SetVisibility(ESlateVisibility::Hidden);//设置为不可见
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            //订阅委托
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
/// 绘制准星
/// </summary>
void ASTUGameHUD::DrawCrossHair() 
{
    //获取屏幕大小
    //int32 SizeX = Canvas->SizeX;
    //int32 SizeY = Canvas->SizeY;
    const TInterval<float> Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);

    const float HalfLineSize = 10.0f;
    const float LineThickness = 2.0f;
    const FLinearColor LineColor = FLinearColor::Green;
    //绘制线条 参数：屏幕上的起点（x）；屏幕上的起点（y）；屏幕上的起点（x）;屏幕上的起点（y）；线条颜色；线条粗细
    //水平线
    DrawLine(Center.Min - HalfLineSize, Center.Max, Center.Min + HalfLineSize, Center.Max, LineColor, LineThickness);
    //垂直线
    DrawLine(Center.Min, Center.Max - HalfLineSize, Center.Min, Center.Max + HalfLineSize, LineColor, LineThickness);
}
