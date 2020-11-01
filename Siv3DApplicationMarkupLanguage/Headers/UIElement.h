#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElementTypeInfo;
    class UIElement;
    using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;

    // UI�v�f�̊��N���X
    class UIElement 
    {
        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

    public:
        // ���̌^�̃v���p�e�B����datas�ɒǉ�����B
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        // �����̃N���X����UIElement�𐶐�����B
        static std::shared_ptr<UIElement> create(const String& className);

        // UIElement�̏���ǂݍ���
        static void initialize();

        UIElement();

        void setProperty(const String& propName, const String& value);

        virtual void draw() {};
    };
}