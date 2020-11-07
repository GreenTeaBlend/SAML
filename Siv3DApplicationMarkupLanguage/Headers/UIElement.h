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

        SpElement m_parent;
        Array<SpElement> m_children;

        UIPanel& m_panel;

        // �q�v�f�̒ǉ��E�폜�����B
        void appendChild(const SpElement& child);
        void removeChild(const UIElement* child);

        // UIElement�̏���ǂݍ���
        static void initialize();

        // �����̃N���X����UIElement�𐶐�����B
        static std::shared_ptr<UIElement> create(const String& className, UIPanel& panel, const SpElement& parent = nullptr);

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

        // �`�悷��B
        virtual void draw() { }

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

        const SpElement& getParent() const { return m_parent; }

        /// <summary>
        /// ����Element��������Panel
        /// </summary>
        UIPanel& getPanel() const { return m_panel; }

        void setProperty(const String& propName, const String& value);

#if false // SamlController�̎����ɓo�^���鏈�����K�v�Ȃ̂ŁA�v���p�e�B�̌��J�͂��������񂵁B
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        ///// <summary>
        ///// �w�肳�ꂽ�^��UIElement�𐶐����A�q�v�f�Ƃ��ēo�^����B
        ///// </summary>
        ///// <typeparam name="T"></typeparam>
        ///// <param name="parent"></param>
        ///// <returns></returns>
        //template <class T>
        //std::shared_ptr<T> createChild() {
        //    auto sp = std::shared_ptr<T>(new T(m_panel));
        //    sp->m_parent = 

        //    if (parent != nullptr) {
        //        parent->appendChild(sp);
        //    }

        //    return sp;
        //}
    };
}