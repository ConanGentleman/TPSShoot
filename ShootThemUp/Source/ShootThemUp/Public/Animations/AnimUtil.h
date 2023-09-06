#pragma once
class AnimUtils
{
public:
	template<typename T>
    static T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (!Animation) return nullptr;
        //获取动画通知
        const auto NotifyEvents = Animation->Notifies;
        for (auto NotifyEvent : NotifyEvents)
        {
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);
            if (AnimNotify) //指针转换成功，则是我们需要的通知
            {
                ////获取委托，并绑定函数
                //AnimNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
                return AnimNotify;
            }
        }
        return nullptr;
    }
};