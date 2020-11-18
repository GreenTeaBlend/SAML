#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    class Button : public UIElement
    {
        String m_text;

        Font m_font;

        bool m_isMouseOvered;

    public:
        Button();

        const String& getText() const { return m_text; }
        void setText(const String& text) { m_text = text; }

        /// <summary>
        /// ï`âÊÇ∑ÇÈÅB
        /// </summary>
        bool draw(bool mouseOverEnable = true) override;
    };
}