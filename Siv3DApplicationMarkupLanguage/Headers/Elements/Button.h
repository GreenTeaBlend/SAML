#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    class Button : public UIElement
    {
        String m_text;

        Font m_font;

    protected:

        void onDraw() override;

    public:
        Button();

        const String& getText() const { return m_text; }
        void setText(const String& text) { m_text = text; }
    };
}