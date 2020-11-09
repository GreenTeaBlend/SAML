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
    // このUIElementの型情報を取得する。
    auto typeInfo = elementDatas.find(className());
    if (typeInfo == elementDatas.end()) {
        throw s3d::Error(U"The element info of \"{}\" was not found. ({})"_fmt(className(), Unicode::Widen(__FUNCTION__)));
    }

    // 値を設定する。
    typeInfo->second->getPropertySetter(this, propName, value);
}

void UIElement::setParent(std::shared_ptr<UIElement> parent)
{
    if (parent != nullptr)
    {
        if (m_parent != nullptr) {
            throw Error(U"UIElementのParentは変更できません。 name={}, m_parent={}, parent={} @UIElement::setParent"_fmt(this->className(), m_parent->className(), parent->className()));
        }

        parent->m_children.push_back(this);
    }
    else
    {
        if (m_parent == nullptr) {
            throw Error(U"UIElementのParentはすでにNULLです。 name={} @UIElement::setParent"_fmt(this->className()));
        }

#ifndef _DEBUG
        // m_parentのchildrenから自身を削除
        for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ++it ) {
            if (*it == this) {
                m_parent->m_children.erase(it);
                break;
            }
        }
#else
        // Debugモードなら、m_parentのchildrenに自身が1つのみ登録されていることを確かめる。
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
            throw Error(U"引数のChildが見つかりませんでした。 @UIElement::setParent");
        }
        else if (deleteCount >= 2) {
            throw Error(U"2つ以上のChildrenがヒットしました @UIElement::setParent");
        }
#endif
    }

    m_parent = parent;
}