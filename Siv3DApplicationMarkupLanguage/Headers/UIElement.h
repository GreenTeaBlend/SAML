#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI {
    class UIElement;
}
using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;

namespace s3d::SamlUI
{
    class PropertyNotImplementedException : public std::runtime_error
    {
    public:
        PropertyNotImplementedException(const char* elementName, const char* propertyName);
    };

    class UIElementData
    {
        std::function<std::shared_ptr<class UIElement>()> m_instanceFactory;
        HashTable<String, PropertySetter> m_propertySetters;

    public:
        UIElementData(std::function<std::shared_ptr<class UIElement>()> factory, HashTable<String, PropertySetter> m_properties);

        std::shared_ptr<class UIElement> create() { return m_instanceFactory(); }
        void setProperty(UIElement* element, const String& propName, const String& value);
    };

    class UIElement 
    {
        static HashTable<String, std::shared_ptr<UIElementData>> elementDatas;

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        static std::shared_ptr<UIElement> create(const String& className);

        static void initialize();

        UIElement();

        void setProperty(const String& propName, const String& value);

        virtual void draw() {};
    };
}