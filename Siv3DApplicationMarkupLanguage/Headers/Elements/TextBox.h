#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"

namespace s3d::SamlUI
{
    class TextBox : RectElement
    {
    public:

        TextBox();

        virtual void SetProperty(const String& name, const String& value);

        virtual void draw();
    };
}