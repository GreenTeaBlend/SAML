#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI {
    class UIElement;
    class UIPanel;
}

namespace s3d::SamlUI 
{
    using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;
    using ElementFactory = std::function<std::shared_ptr<class UIElement>(UIPanel& panel)>;

    // UIElementの型情報
    class UIElementTypeInfo
    {
        ElementFactory m_instanceFactory;
        HashTable<String, PropertySetter> m_propertySetters;

    public:
        UIElementTypeInfo(ElementFactory factory, HashTable<String, PropertySetter> m_properties);

        std::shared_ptr<class UIElement> create(UIPanel& panel) { return m_instanceFactory(panel); }

        // 引数のUIElementのプロパティに値をセットする。
        void getPropertySetter(UIElement* element, const String& propName, const String& value);
    };
}