#include "UIElement.h"
#include "UIPanel.h"
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

    std::shared_ptr<UIElement> element = elementDatas.at(className)->create(panel);

    if (parent != nullptr) {
        element->setParent(parent);
    }

    return element;
}

UIElement::UIElement(UIPanel& panel):
    m_panel(panel),
    m_parent(),
    m_name()
{
}

UIElement::~UIElement() 
{
    if (m_parent != nullptr) {
        setParent(nullptr);
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
    // ����UIElement�̌^�����擾����B
    auto typeInfo = elementDatas.find(className());
    if (typeInfo == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found. ({})"_fmt(className(), Unicode::Widen(__FUNCTION__)));
    }

    // �l��ݒ肷��B
    typeInfo->second->getPropertySetter(this, propName, value);
}

void UIElement::setParent(std::shared_ptr<UIElement> parent)
{
    if (parent != nullptr)
    {
        if (m_parent != nullptr) {
            throw Error(U"UIElement��Parent�͕ύX�ł��܂���B name={}, m_parent={}, parent={} @UIElement::setParent"_fmt(this->className(), m_parent->className(), parent->className()));
        }

        parent->m_children.push_back(this);
    }
    else
    {
        if (m_parent == nullptr) {
            throw Error(U"UIElement��Parent�͂��ł�NULL�ł��B name={} @UIElement::setParent"_fmt(this->className()));
        }

#ifndef _DEBUG
        // m_parent��children���玩�g���폜
        for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ++it ) {
            if (*it == this) {
                m_parent->m_children.erase(it);
                break;
            }
        }
#else
        // Debug���[�h�Ȃ�Am_parent��children�Ɏ��g��1�̂ݓo�^����Ă��邱�Ƃ��m���߂�B
        int deleteCount = 0;
        for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ) {
            if (*it == this) {
                it = m_parent->m_children.erase(it);
                ++deleteCount;
            }
            else {
                ++it;
            }
        }

        if (deleteCount == 0) {
            throw Error(U"������Child��������܂���ł����B @UIElement::setParent");
        }
        else if (deleteCount >= 2) {
            throw Error(U"2�ȏ��Children���q�b�g���܂��� @UIElement::setParent");
        }
#endif
    }

    m_parent = parent;
}