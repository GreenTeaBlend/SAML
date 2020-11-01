#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI {
    class UIElement;
}
using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;

namespace s3d::SamlUI 
{
    // UIElementの型情報
    class UIElementTypeInfo
    {
        std::function<std::shared_ptr<class UIElement>()> m_instanceFactory;
        HashTable<String, PropertySetter> m_propertySetters;

    public:
        UIElementTypeInfo(std::function<std::shared_ptr<class UIElement>()> factory, HashTable<String, PropertySetter> m_properties);

        std::shared_ptr<class UIElement> create() { return m_instanceFactory(); }

        // 引数のUIElementのプロパティに値をセットする。
        void getPropertySetter(UIElement* element, const String& propName, const String& value);
    };
}