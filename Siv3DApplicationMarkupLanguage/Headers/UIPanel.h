#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElement;
    class BindableObject;

    /// <summary>
    /// UIElement�C���X�^���X��ێ�����Manager�N���X�B
    /// </summary>
    class UIPanel
    {
        // ����Panel�ɑ�����SUIElement
        Array<std::shared_ptr<UIElement>> m_elements;

        std::shared_ptr<UIElement> m_rootElement;

        // �}�E�X�I�[�o�[���Ă���v�f (�őO�ʂ̂���)
        std::shared_ptr<UIElement> m_mouseOveredElement;

        // �t�H�[�J�X���̗v�f
        std::shared_ptr<UIElement> m_focusingElement;

        // �`��̈�(�N���C�A���g���W)
        RectF m_rect;

        // ���ʎq�t����UIElement
        s3d::HashTable<String, std::shared_ptr<UIElement>> m_namedElements;

        std::shared_ptr<UIElement> createElementRecursively(const XMLElement& xmlElement, const std::shared_ptr<UIElement>& parent);

        UIPanel(const UIPanel&) = delete;
        const UIPanel& operator=(const UIPanel&) = delete;

        UIPanel();

    public:
        /// <summary>
        /// �e���v���[�g��UIElement�����[�g�v�f�Ƃ��AUIPanel�𐶐�����B
        /// </summary>
        /// <typeparam name="T">UIElement</typeparam>
        template <class T>
        static std::shared_ptr<UIPanel> create()
        {
            auto panel = std::shared_ptr<UIPanel>(new UIPanel);
            panel->m_rootElement = std::shared_ptr<UIElement>(new T(*panel));
            panel->m_elements.push_back(panel->m_rootElement);
            return panel;
        }

        /// <summary>
        /// XML�R�[�h�����Ƃ�UIElement�𐶐�����B
        /// </summary>
        /// <param name="error">���s���̃G���[���e</param>
        static std::shared_ptr<UIPanel> create(String xml, String* error = nullptr);

        /// <summary>
        /// NameAttribute���ݒ肳��Ă���UIElement���擾����B
        /// </summary>
        const std::shared_ptr<UIElement> getElement(const String& name) const {
            auto it = m_namedElements.find(name);
            return it != m_namedElements.end() ? it->second : nullptr;
        }

        /// <summary>
        /// ���[�g�v�f���擾����B
        /// </summary>
        const std::shared_ptr<UIElement>& getRoot() const { return m_rootElement; }

        /// <summary> �ʒu�ƃT�C�Y�̊�ƂȂ��`���擾���� </summary>
        const RectF& getRect() const { return m_rect; }
        /// <summary> �ʒu�ƃT�C�Y�̊�ƂȂ��`��ݒ肷�� </summary>
        void setRect(const RectF& rect);

        // �eUIElement�̃N���b�N����Ȃǂ̏����ƕ`����s���B
        void drawUpdate();

        /// <summary>
        ///  �}�E�X�I�[�o�[���Ă���v�f (�őO�ʂ̂���)
        /// </summary>
        const std::shared_ptr<UIElement>& getMouseOveredElement() const { return m_mouseOveredElement; }

        /// <summary>
        /// �t�H�[�J�X���̗v�f
        /// </summary>
        const std::shared_ptr<UIElement>& getFocusingElementconst() const { return m_focusingElement; }

        void setDataContext(BindableObject& data);
    };
}