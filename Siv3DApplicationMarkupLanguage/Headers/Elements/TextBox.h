#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"

namespace s3d::SamlUI
{
    class TextBox : public RectElement
    {
        Font m_font;

        String m_text;

        bool m_isFocused;

        int m_cursorPos;
    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        TextBox();

        virtual bool draw() override;

        virtual void onFocusStart() override;
        virtual void onFocusEnd() override;

        virtual void onMouseOverStart() override;
        virtual void onMouseOverEnd() override;
    };
}