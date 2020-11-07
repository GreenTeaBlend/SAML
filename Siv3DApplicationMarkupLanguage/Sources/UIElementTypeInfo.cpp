#include "UIElementTypeInfo.h"

using namespace s3d;
using namespace SamlUI;

UIElementTypeInfo::UIElementTypeInfo(ElementFactory factory, HashTable<String, PropertySetter> m_properties) :
    m_instanceFactory(factory),
    m_propertySetters(m_properties)
{

}

void UIElementTypeInfo::getPropertySetter(UIElement* element, const String& propName, const String& value)
{
    if (m_propertySetters.find(propName) == m_propertySetters.end()) {
        throw s3d::Error(U"The property setter for \"{}\" was not found."_fmt(propName));
    }

    m_propertySetters.at(propName)(element, value);
}