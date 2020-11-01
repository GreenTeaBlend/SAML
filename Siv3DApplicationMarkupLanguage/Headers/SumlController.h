#pragma once
#include <Siv3D.hpp>

namespace Suml
{
    class UIElement;

    // XMLベースのUI描画クラス
    class SumlController
    {
        bool m_isValid;

        String m_error;

        Array<std::shared_ptr<UIElement>> m_elements;

        // 引数のxmlElementをUIElementとしてParseする。isNull()==trueならfalseを返す。
        bool parseXmlElement(XMLElement* xmlElement);

    public:
        SumlController();

        void parse(String xml);

        // 最後に実行したparseに成功したか否か。
        bool isValid() const { return m_isValid; }

        // 最後に実行したparseで発生したエラー。
        String getError() { return m_error; }

        void draw();
    };
}