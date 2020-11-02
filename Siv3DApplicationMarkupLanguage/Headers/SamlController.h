#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElement;

    // XMLベースのUI描画クラス
    class SamlController
    {
        bool m_isValid;

        String m_error;

        Array<std::shared_ptr<UIElement>> m_elements;

        // 引数のxmlElementをUIElementとしてParseする。isNull()==trueならfalseを返す。
        bool parseXmlElement(XMLElement* xmlElement);
        std::shared_ptr<UIElement> createElement(const XMLElement& xmlElement);

        std::shared_ptr<UIElement> m_mouseOveredElement;
        std::shared_ptr<UIElement> m_focusingElement;

        s3d::HashTable<String, std::shared_ptr<UIElement>> m_namedElements;

    public:
        SamlController();

        void parse(String xml);

        // 最後に実行したparseに成功したか否か。
        bool isValid() const { return m_isValid; }

        // 最後に実行したparseで発生したエラー。
        String getError() { return m_error; }

        // NameAttributeが設定されているUIElementをしゅとくする。
        const std::shared_ptr<UIElement> getElement(const String& name) const { return m_namedElements.at(name); }

        void draw();
    };
}