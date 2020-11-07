#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElement;
    using SpElement = std::shared_ptr<UIElement>;

    /// <summary>
    /// UIElement�C���X�^���X��ێ�����N���X�B
    /// </summary>
    class UIPanel
    {
        // ����Panel�ɑ�����SUIElement
        Array<std::shared_ptr<UIElement>> m_elements;

        std::shared_ptr<UIElement> m_rootElement;

        //// ������xmlElement��UIElement�Ƃ���Parse����BisNull()==true�Ȃ�false��Ԃ��B
        //bool parseXmlElement(XMLElement* xmlElement);
        //std::shared_ptr<UIElement> createElement(const XMLElement& xmlElement);

        // �}�E�X�I�[�o�[���Ă���v�f (�őO�ʂ̂���)
        std::shared_ptr<UIElement> m_mouseOveredElement;

        // �t�H�[�J�X���̗v�f
        std::shared_ptr<UIElement> m_focusingElement;

        // ���ʎq�t����UIElement
        s3d::HashTable<String, std::shared_ptr<UIElement>> m_namedElements;

        UIPanel(const UIPanel&) = delete;
        const UIPanel& operator=(const UIPanel&) = delete;

        UIPanel();

    public:
        /// <summary>
        /// �e���v���[�g��UIElement�����[�g�v�f�Ƃ��AUIPanel�𐶐�����B
        /// </summary>
        /// <typeparam name="T">UIElement</typeparam>
        template <class T>
        static UIPanel* create()
        {
            auto* panel = new UIPanel();
            panel->m_rootElement = std::shared_ptr<UIElement>(new T(*panel));
            panel->m_elements.push_back(panel->m_rootElement);
            return panel;
        }

        /// <summary>
        /// XML�R�[�h�����Ƃ�UIElement�𐶐�����B
        /// </summary>
        static UIPanel* create(String xml);

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
        /// <returns></returns>
        const std::shared_ptr<UIElement>& getRoot() const { return m_rootElement; }

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
    };
}