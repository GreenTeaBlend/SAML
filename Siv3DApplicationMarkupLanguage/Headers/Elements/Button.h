#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"

namespace s3d::SamlUI
{
    class Button : RectElement
    {
        String m_text;
    public:

        const String& getText() const { return m_text; }
        void setText(const String& text) { m_text = text; }

        Button();

        virtual void SetProperty(const String& name, const String& value);

        virtual void draw();
    };
}