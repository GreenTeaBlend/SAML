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

        // ���ʗp��UI��
        String m_name;

    public:
        // ���̌^�̃v���p�e�B����datas�ɒǉ�����B
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        // �����̃N���X����UIElement�𐶐�����B
        static std::shared_ptr<UIElement> create(const String& className);

#if false // SamlController�̎����ɓo�^���鏈�����K�v�Ȃ̂ŁA�v���p�e�B�̌��J�͂��������񂵁B
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        // UIElement�̏���ǂݍ���
        static void initialize();

        UIElement();

        void setProperty(const String& propName, const String& value);

        // �`�悷��B�}�E�X�I�[�o�[����Ă�����true��Ԃ��B
        virtual bool draw() { return false; }

        virtual void onClicked() {}

        // �}�E�X�N���b�N�Ȃǂɂ��Focus��ԊJ�n
        virtual void onFocusStart() {}
        virtual void onFocusing() {}
        virtual void onFocusEnd() {}

        virtual void onMouseOverStart() {}
        virtual void onMouseOvering() {}
        virtual void onMouseOverEnd() {}
    };
}