// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUCoreTypes.h"
#include "STUWeaponComponent.generated.h"

class ASTUBaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTUWeaponComponent();

	virtual void StartFire();
	void StopFire();
    virtual void NextWeapon();
    void Reload();

    bool GetCurrentWeaponUIData(FWeaponUIData& UIData) const;
    bool GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const;

    bool TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType,int32 ClipsAmount);
    bool NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType);

    void Zoom(bool Enabled);

protected:
    //武器属性
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<FWeaponData> WeaponData;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponArmorySocketName = "ArmorySocket";

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* EquipAnimMontage;

    UPROPERTY()
    ASTUBaseWeapon* CurrentWeapon = nullptr;

    //武器数组
    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    int32 CurrentWeaponIndex = 0;

	// Called when the game starts
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    bool CanFire() const;
    bool CanEquip() const;
    //设置CurrentWeapon 并将武器附加到角色手上
    void EquipWeapon(int32 WeaponIndex);

private:

	UPROPERTY()
    UAnimMontage* CurrentReloadAnimMontage=nullptr;

	
    bool EquipAnimInProgress = false;
    bool ReloadAnimInProgress = false;

    //订阅动画事件委托
    void InitAnimations();
	//生成武器模型
    void SpawnWeapons();
	//将模型绑定到插槽上（骨骼）
    void AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USkeletalMeshComponent* SceneComponent, const FName& SocketName);

	//播放切枪动画
    void PlayAnimMontage(UAnimMontage* Animation);

    void OnEquipFinished(USkeletalMeshComponent* MeshComponent);
    void OnReloadFinished(USkeletalMeshComponent* MeshComponent);

	bool CanReload() const;

    void OnClipEmpty(ASTUBaseWeapon* AmmoEmptyWeapon);
    void ChangeClip();

    
};
