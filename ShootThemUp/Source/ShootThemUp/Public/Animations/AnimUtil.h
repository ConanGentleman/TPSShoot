#pragma once
class AnimUtils
{
public:
	template<typename T>
    static T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (!Animation) return nullptr;
        //��ȡ����֪ͨ
        const auto NotifyEvents = Animation->Notifies;
        for (auto NotifyEvent : NotifyEvents)
        {
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);
            if (AnimNotify) //ָ��ת���ɹ�������������Ҫ��֪ͨ
            {
                ////��ȡί�У����󶨺���
                //AnimNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
                return AnimNotify;
            }
        }
        return nullptr;
    }
};