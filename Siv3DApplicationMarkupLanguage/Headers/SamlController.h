#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElement;

    // XML�x�[�X��UI�`��N���X
    class SamlController
    {
        bool m_isValid;

        String m_error;

        Array<std::shared_ptr<UIElement>> m_elements;

        // ������xmlElement��UIElement�Ƃ���Parse����BisNull()==true�Ȃ�false��Ԃ��B
        bool parseXmlElement(XMLElement* xmlElement);
        std::shared_ptr<UIElement> createElement(const XMLElement& xmlElement);

        std::shared_ptr<UIElement> m_mouseOveredElement;
        std::shared_ptr<UIElement> m_focusingElement;

        s3d::HashTable<String, std::shared_ptr<UIElement>> m_namedElements;

    public:
        SamlController();

        void parse(String xml);

        // �Ō�Ɏ��s����parse�ɐ����������ۂ��B
        bool isValid() const { return m_isValid; }

        // �Ō�Ɏ��s����parse�Ŕ��������G���[�B
        String getError() { return m_error; }

        // NameAttribute���ݒ肳��Ă���UIElement������Ƃ�����B
        const std::shared_ptr<UIElement> getElement(const String& name) const { return m_namedElements.at(name); }

        void draw();
    };
}