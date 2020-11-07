#include "UIElement.h"
#include "Controller.h"
#include "UIElementTypeInfo.h"
#include "Elements/Button.h"
#include "Elements/TextBox.h"
#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

bool UIElement::hasInitialized = false;

#define ELEMENT_DATA(name)\
{\
std::function<std::shared_ptr<class UIElement>(UIPanel&)> factory = [&](UIPanel& panel) { return std::shared_ptr<UIElement>((UIElement*)(new name##(panel))); }; \
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

    hasInitialized = true;
}

std::shared_ptr<UIElement> UIElement::create(const String& className, UIPanel& panel, const SpElement& parent)
{
    if (elementDatas.find(className) == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found."_fmt(className));
    }

    return elementDatas.at(className)->create(panel);
}

UIElement::UIElement(UIPanel& panel):
    m_panel(panel),
    m_parent()
{
}

UIElement::~UIElement() 
{
    if (m_parent != nullptr) {
        m_parent->removeChild(this);
    }
}

bool UIElement::isMouseOvered() const
{
    return m_panel.getMouseOveredElement().get() == this; 
}

bool UIElement::isFocusing() const
{
    return m_panel.getFocusingElementconst().get() == this; 
}

void UIElement::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"Name",
        [&](UIElement* elm, const String& value) {
            ((Button*)elm)->m_name = value;
        }));
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

void UIElement::appendChild(const SpElement& child)
{
    m_children.push_back(child);
}

void UIElement::removeChild(const UIElement* child)
{
#if _DEBUG
    bool foundChild = false;

    for (auto it = m_children.begin(); it != m_children.end(); ) {
        if (it->get() == child) {
            it = m_children.erase(it);

            if (!foundChild) {
                foundChild = true;
            }
            else {
                throw Error(U"2�ȏ��Children���q�b�g���܂��� @UIElement::removeChild");
            }
        }
        else {
            ++it;
        }
    }

    if (!foundChild) {
        throw Error(U"������Child��������܂���ł����B @UIElement::removeChild");
    }
#else
    std::remove(m_children.begin(), , child) == m_children.end()
#endif
}