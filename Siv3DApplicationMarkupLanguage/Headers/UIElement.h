#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI {
    class UIElement;
}
using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&, const s3d::String&)>;

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
    };

    class UIElement 
    {
        static HashTable<String, std::shared_ptr<UIElementData>> m_elementDatas;

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        static void initialize();

        virtual void SetProperty(const String& name, const String& value);

        virtual void draw() {};
    };
}