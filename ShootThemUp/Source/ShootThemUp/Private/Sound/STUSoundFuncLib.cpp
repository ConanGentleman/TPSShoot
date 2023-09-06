// Shoot Them Up Game, All Rights Reserved.


#include "Sound/STUSoundFuncLib.h"
#include "Sound/SoundClass.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUSoundFuncLib, All, All);

/// <summary>
/// 设置音频音量
/// </summary>
/// <param name="SoundClass"></param>
/// <param name="Volume"></param>
void USTUSoundFuncLib::SetSoundClassVolume(USoundClass* SoundClass, float Volume)
{
    if (!SoundClass) return;
    
    SoundClass->Properties.Volume = FMath::Clamp(Volume,0.0f,1.0f);
    UE_LOG(LogSTUSoundFuncLib, Display, TEXT("Sound class volume was changed: %s = %f"),*SoundClass->GetName(),SoundClass->Properties.Volume);
}
/// <summary>
/// 重置音频音量(在静音和开启声音来回切换)
/// </summary>
/// <param name="SoundClass"></param>
void USTUSoundFuncLib::ToggleSoundClassVolume(USoundClass* SoundClass) 
{
    if (!SoundClass) return;
    const auto NextVolume = SoundClass->Properties.Volume > 0.0f ? 0.0f : 1.0f;
    SetSoundClassVolume(SoundClass, NextVolume);
}