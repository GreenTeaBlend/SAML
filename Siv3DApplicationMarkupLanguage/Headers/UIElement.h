#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIPanel;
    class UIElementTypeInfo;
    class UIElement;
    using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;
    using SpElement = std::shared_ptr<UIElement>;

    /// <summary>
    /// UI�v�f�̊��N���X
    /// </summary>
    class UIElement 
    {
        friend class UIPanel;

        static bool hasInitialized;

        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

        // ���ʗp��UI��
        String m_name;

        std::shared_ptr<UIElement> m_parent;
        Array<UIElement*> m_children;

        UIPanel& m_panel;

        String m_className;

        // �e�v�f�̐ݒ� 
        // �����_�ł͐�����̕ύX�͑z�肵�Ă��炸�A�܂��e�v�f���珇�ɐe�q�֌W���m�肳����B(�q���������v�f�𑼗v�f�̎q�ɂ��Ȃ�)
        void setParent(std::shared_ptr<UIElement> parent);

        // UIElement�̏���ǂݍ���
        static void initialize();

        // �����̃N���X����UIElement�𐶐�����B
        static std::shared_ptr<UIElement> create(const String& className, UIPanel& panel, const SpElement& parent);

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

        // �`�悷��B
        virtual void draw() { }

        // ���̃C���X�^���X�̃N���X����Ԃ��B
        String className() {
            if (m_className.length() == 0) {
                // "class Saml::UIElement::Button" �̂悤�ȕ����񂪎擾�����̂ŁA���O��Ԃ������������ȃN���X���ɕϊ�����B
                m_className = Unicode::Widen(typeid(*this).name());
                auto colonIndex = m_className.lastIndexOf(U":");
                if (colonIndex != String::npos) {
                    m_className = m_className.substr(colonIndex + 1); // �R����������擾
                }
                else {
                    m_className = m_className.substr(6); // �擪��"class "������擾
                }
            }
            return m_className;
        }

    protected:

        UIElement(UIPanel& panel);

        virtual void onMouseOverChanged() {};
        virtual void onFocusChanged() {};

        // ���̌^�̃v���p�e�B����datas�ɒǉ�����B
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

    public:
        ~UIElement();

        // UI�̑O��֌W���l�������A�}�E�X�I�[�o�[����Ă��邩��Ԃ��B
        virtual bool intersectsMouse() { return false; }

        // �}�E�X�I�[�o�[����Ă��邩�BUI�̑O��֌W���l������B
        bool isMouseOvered() const;
        bool isFocusing() const;

        const std::shared_ptr<UIElement>& getParent() const { return m_parent; }

        /// <summary>
        /// ����Element��������Panel
        /// </summary>
        UIPanel& getPanel() const { return m_panel; }

        void setProperty(const String& propName, const String& value);

#if false // SamlController�̎����ɓo�^���鏈�����K�v�Ȃ̂ŁA�v���p�e�B�̌��J�͂��������񂵁B
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        const Array<UIElement*>& getChildren() const { return m_children; }
    };
}