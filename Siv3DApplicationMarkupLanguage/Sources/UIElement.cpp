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
std::string nameTmp = #name; /* ���� U#name �̂悤�Ƀ}�N���ɔz�u����ƁA�R���p�C���͒ʂ�̂ɃG���[���o�͂����*/ \
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
    // "class Saml::UIElement::Button" �̂悤�ȕ����񂪎擾�����̂ŁA���O��Ԃ������������ȃN���X���ɕϊ�����B
    String className = Unicode::Widen(typeid(*this).name());
    auto colonIndex = className.lastIndexOf(U":");
    if (colonIndex != String::npos) {
        className = className.substr(colonIndex + 1); // �R����������擾
    }
    else {
        className = className.substr(6); // �擪��"class "������擾
    }

    // ����UIElement�̌^�����擾����B
    if (elementDatas.find(className) == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found. ({})"_fmt(className, Unicode::Widen(__FUNCTION__)));
    }
    const std::shared_ptr<UIElementTypeInfo>& typeInfo = elementDatas.at(className);

    // �l��ݒ肷��B
    typeInfo->getPropertySetter(this, propName, value);
}