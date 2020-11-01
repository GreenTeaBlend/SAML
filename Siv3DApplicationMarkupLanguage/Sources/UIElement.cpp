#include "UIElement.h"
#include "Elements/Button.h"
#include "Elements/TextBox.h"
#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

#define ELEMENT_DATA(name)\
{\
std::function<std::shared_ptr<class UIElement>()> factory = [&]() { return std::shared_ptr<UIElement>((UIElement*)(new name##())); }; \
HashTable<String, PropertySetter> properties; \
name##::enumratePropertyData(&properties); \
std::shared_ptr<UIElementData> elementData{ new UIElementData(factory, properties) }; \
std::string nameTmp = #name; /* 直接 U#name のようにマクロに配置すると、コンパイルは通るのにエラーが出力される*/ \
elementDatas.insert(std::make_pair(Unicode::Widen(nameTmp) , elementData)); \
}

HashTable<String, std::shared_ptr<UIElementData>> UIElement::elementDatas{};

PropertyNotImplementedException::PropertyNotImplementedException(const char* elementName, const char* propertyName)
    : runtime_error("There is no property named \"" + std::string(propertyName) + "\" in \"" + std::string(elementName) + "\".")
{}

UIElementData::UIElementData(std::function<std::shared_ptr<class UIElement>()> factory, HashTable<String, PropertySetter> m_properties) :
    m_instanceFactory(factory),
    m_propertySetters(m_properties)
{

}

void UIElementData::setProperty(UIElement* element, const String& propName, const String& value)
{
    if (m_propertySetters.find(propName) == m_propertySetters.end()) {
        throw s3d::Error(U"The property setter for \"{}\" was not found."_fmt(propName));
    }

    m_propertySetters.at(propName)(element, value); 
}

void UIElement::initialize()
{
    ELEMENT_DATA(UIElement);
    ELEMENT_DATA(RectElement);
    ELEMENT_DATA(Button);
    ELEMENT_DATA(TextBox);
}

std::shared_ptr<UIElement> UIElement::create(const String& className)
{
    if (elementDatas.find(className) == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found."_fmt(className));
    }

    return elementDatas.at(className)->create();
}

UIElement::UIElement()
{

}

void UIElement::enumratePropertyData(HashTable<String, PropertySetter>*)
{

}

void UIElement::setProperty(const String& propName, const String& value)
{
    // "class Saml::UIElement::Button" のような文字列が取得されるので、名前空間も除いた純粋なクラス名に変換する。
    String className = Unicode::Widen(typeid(*this).name());
    auto colonIndex = className.lastIndexOf(U":");
    if (colonIndex != String::npos) {
        className = className.substr(colonIndex + 1); // コロンより後を取得
    }
    else {
        className = className.substr(6); // 先頭の"class "より後を取得
    }

    if (elementDatas.find(className) == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found. ({})"_fmt(className, Unicode::Widen(__FUNCTION__)));
    }

    const std::shared_ptr<UIElementData>& elmData = elementDatas.at(className);
    elmData->setProperty(this, propName, value);
}