#include "UIElement.h"
#include "Elements/Button.h"
#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

#define ELEMENT_DATA(name)\
std::function<std::shared_ptr<class UIElement>()> factory = [&]() { return std::shared_ptr<UIElement>((UIElement*)(new name##())); }; \
HashTable<String, PropertySetter> properties; \
name##::enumratePropertyData(&properties); \
std::shared_ptr<UIElementData> elementData{ new UIElementData(factory, properties) }; \
std::string nameTmp = #name; /* 直接 U#name のようにマクロに配置すると、コンパイルは通るのにエラーが出力される*/ \
m_elementDatas.insert(std::make_pair(Unicode::Widen(nameTmp) , elementData)); \

HashTable<String, std::shared_ptr<UIElementData>> UIElement::m_elementDatas{};

PropertyNotImplementedException::PropertyNotImplementedException(const char* elementName, const char* propertyName)
    : runtime_error("There is no property named \"" + std::string(propertyName) + "\" in \"" + std::string(elementName) + "\".")
{}

UIElementData::UIElementData(std::function<std::shared_ptr<class UIElement>()> factory, HashTable<String, PropertySetter> m_properties) :
    m_instanceFactory(factory)
{

}

void UIElement::initialize()
{
    //std::function<std::shared_ptr<class UIElement>()> factory = [&]() { return std::shared_ptr<UIElement>((UIElement*)(new RectElement)); };
    //HashTable<String, PropertySetter> properties;
    //RectElement::enumratePropertyData(&properties);
    //std::shared_ptr<UIElementData> elementData{ new UIElementData(factory, properties) };
    //m_elementDatas.insert(std::make_pair(U"RectElement", elementData));

    ELEMENT_DATA(Button)
}

void UIElement::enumratePropertyData(HashTable<String, PropertySetter>*)
{

}

void UIElement::SetProperty(const String& name, const String& value) {
    throw PropertyNotImplementedException(typeid(*this).name(), name.narrow().c_str());
}