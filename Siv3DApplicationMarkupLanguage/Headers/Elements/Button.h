#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    class Button : UIElement
    {
        Vec2 m_position;

        Vec2 m_size;

    public:

        Button();

        virtual void SetProperty(const String& name, const String& value);

        virtual void draw();
    };
}