#include "UIElement.h"
#include "UIElementTypeInfo.h"
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
std::shared_ptr<UIElementTypeInfo> elementData{ new UIElementTypeInfo(factory, properties) }; \
std::string nameTmp = #name; /* 直接 U#name のようにマクロに配置すると、コンパイルは通るのにエラーが出力される*/ \
elementDatas.insert(std::make_pair(Unicode::Widen(nameTmp) , elementData)); \
}

HashTable<String, std::shared_ptr<UIElementTypeInfo>> UIElement::elementDatas{};

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

    // このUIElementの型情報を取得する。
    if (elementDatas.find(className) == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found. ({})"_fmt(className, Unicode::Widen(__FUNCTION__)));
    }
    const std::shared_ptr<UIElementTypeInfo>& typeInfo = elementDatas.at(className);

    // 値を設定する。
    typeInfo->getPropertySetter(this, propName, value);
}