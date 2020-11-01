#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"

namespace s3d::SamlUI
{
    class TextBox : public RectElement
    {
    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        TextBox();

        virtual void draw();
    };
}