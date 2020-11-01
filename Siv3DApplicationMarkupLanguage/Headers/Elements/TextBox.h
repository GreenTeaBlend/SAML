#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    class TextBox : UIElement
    {
        Vec2 m_position;

        Vec2 m_size;

    public:

        TextBox();

        virtual void SetProperty(const String& name, const String& value);

        virtual void draw();
    };
}