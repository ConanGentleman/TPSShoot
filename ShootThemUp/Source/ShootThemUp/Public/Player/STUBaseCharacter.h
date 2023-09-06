// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STUBaseCharacter.generated.h"

class USTUHealthComponent;
class USTUWeaponComponent;
class USoundCue;


UCLASS()
class SHOOTTHEMUP_API ASTUBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTUBaseCharacter(const FObjectInitializer& ObjInit);//用于将UCharacterMovementComponent转换为USTUCharacterMovementComponent

protected:

	// Called when the game starts or when spawned
	//血量
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUHealthComponent* HealthComponent1;

	////血量文本
 //   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
 //   UTextRenderComponent* HealthTextComponent;

	//武器
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUWeaponComponent* WeaponComponent;

	//死亡动画（蒙太奇）
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float LifeSpanOnDeath = 5.0f;

	//坠落伤害范围
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandedDamageVelocity=FVector2D(900.0f,1200.0f);

	//坠落伤害
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandedDamage = FVector2D(10.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Material")
    FName MaterialColorName = "Paint Color";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* DeathSound;

	virtual void BeginPlay() override;
    virtual void OnDeath();
    virtual void OnHealthChanged(float Health, float HealthDelta);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    virtual void TurnOff() override;
    virtual void Reset() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

	void SetPlayerColor(const FLinearColor& Color);

private:
    // void LookUp(float Amount);
    // void TurnAround(float Amount);

	//动态委托 需添加UFUNCTION()宏
    UFUNCTION()
    void OnGroundLanded(const FHitResult& Hit);

};
