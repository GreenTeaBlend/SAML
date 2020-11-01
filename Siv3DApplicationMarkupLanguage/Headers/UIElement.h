#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class PropertyNotImplementedException : public std::runtime_error
    {
    public:
        PropertyNotImplementedException(const char* elementName, const char* propertyName);
    };

    class UIElement {
    public:
        virtual void SetProperty(const String& name, const String& value);

        virtual void draw() {};
    };
}