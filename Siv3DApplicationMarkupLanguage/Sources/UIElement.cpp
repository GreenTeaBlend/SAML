#include "UIElement.h"
#include "UIPanel.h"
#include "UIElementTypeInfo.h"
#include "Elements/Button.h"
#include "Elements/TextBox.h"
#include "BindableObject.h"

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
    m_dataContext(),
    m_name(),
    m_margin(),
    m_size(),
    m_currentPos(),
    m_currentSize(),
    m_horizontalAlignment(),
    m_verticalAlignment(),
    m_isTransformDirty(true)
{
}

UIElement::~UIElement() 
{
    if (m_parent != nullptr) {
        setParent(nullptr);
    }

    setDataContext(nullptr);
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
            elm->m_name = value;
        }));

    datas->insert(std::make_pair(U"Margin",
        [&](UIElement* elm, const String& value) {
            elm->setMargin(Parse<Vec4>(value));
        }));

    datas->insert(std::make_pair(U"Size",
        [&](UIElement* elm, const String& value) {
            elm->setSize(Parse<Vec2>(value));
        }));

    datas->insert(std::make_pair(U"HorizontalAlignment",
        [&](UIElement* elm, const String& value) {
            auto alignment = StringToHorizontalAlignment(value);
            elm->setHorizontalAlignment(alignment);
        }));

    datas->insert(std::make_pair(U"VerticalAlignment",
        [&](UIElement* elm, const String& value) {
            auto alignment = StringToVerticalAlignment(value);
            elm->setVerticalAlignment(alignment);
        }));
}

void UIElement::onPropertyChangedRecursively(const String& name)
{
    onPropertyChanged(name);

    // 自分の子要素のうち、DataContextが設定されていないもの(親の設定を引き継ぐもの)を再帰的に呼ぶ。
    for (auto* child : m_children) {
        if (child->m_dataContext == nullptr) {
            child->onPropertyChangedRecursively(name);
        }
    }
}

void UIElement::onPropertyChanged(const String& name)
{
}

BindableObject* UIElement::getCurrentDataContext() const
{
    for (const UIElement* element = this; element != nullptr; element = element->m_parent.get()) {
        if (element->m_dataContext != nullptr) {
            return element->m_dataContext;
        }
    }

    return nullptr;
}

void UIElement::setDataContext(BindableObject* data)
{
    if (m_dataContext != data) {
        if (m_dataContext != nullptr) {
            Array<UIElement*>& elements = m_dataContext->m_elements;
            for (auto it = elements.begin(); it != elements.end(); ++it) {
                if (*it != this) {
                    elements.erase(it);
                }
            }
        }

        m_dataContext = data;

        if (m_dataContext != nullptr) {
            m_dataContext->m_elements.push_back(this);
        }
    }
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

void UIElement::setParent(const std::shared_ptr<UIElement>& parent)
{
    if (parent != m_parent) 
    {
        // 前のParentのChildrenから自分を削除する。
        if (m_parent != nullptr) {
#ifndef _DEBUG
            // m_parentのchildrenから自身を削除
            for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ++it) {
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

        // 新しいparentのChildrenに自分を削除する。
        if (m_parent != nullptr)
        {
            m_parent->m_children.push_back(this);
        }

        setTransformDirtyRecursively();
    }
}

void UIElement::setTransformDirtyRecursively()
{
    if (!m_isTransformDirty) 
    {
        m_isTransformDirty = true;

        // TransformChangedEventなど
    }

    for (auto child : getChildren()) {
        child->setTransformDirtyRecursively();
    }
}

void UIElement::updateTransform()
{
    Vec2 parentPos = m_parent != nullptr ? m_parent->getPosition() : getPanel().getRect().pos;
    Vec2 parentSize = m_parent != nullptr ? m_parent->getSize() : getPanel().getRect().size;
    const Vec4 margin = m_margin.has_value() ? m_margin.value() : Vec4::Zero();

    //--------------------------------------------------
    // サイズを決定
    if (m_size.has_value()) {
        m_currentSize = m_size.value();
    }
    else
    {
        if (!m_horizontalAlignment.has_value() || m_horizontalAlignment.value() == HorizontalAlignment::Stretch) {
            m_currentSize.x = parentSize.x - margin.x - margin.z;
        }
        else {
            m_currentSize.x = 0; /*要素の本来のサイズに合わせる*/
        }

        if (!m_verticalAlignment.has_value() || m_verticalAlignment.value() == VerticalAlignment::Stretch) {
            m_currentSize.y = parentSize.y - margin.y - margin.w;
        }
        else {
            m_currentSize.y = 0; /*要素の本来のサイズに合わせる*/
        }
    }

    //--------------------------------------------------
    // 位置(左)を決定
    HorizontalAlignment halign = m_horizontalAlignment.has_value() ? m_horizontalAlignment.value() : HorizontalAlignment::Stretch;
    switch (halign)
    {
    case s3d::SamlUI::HorizontalAlignment::Right:
        m_currentPos.x = parentPos.x + parentSize.x - margin.z - m_currentSize.x;
        break;
    case s3d::SamlUI::HorizontalAlignment::Center:
        m_currentPos.x = parentPos.x + parentSize.x / 2 - m_currentSize.x / 2;
        break;
    case s3d::SamlUI::HorizontalAlignment::Left:
    case s3d::SamlUI::HorizontalAlignment::Stretch:
        m_currentPos.x = parentPos.x + margin.x;
        break;
    default:
        throw Error(U"未定義のHorizontalAlignment \"{}\" @RectElement::updateTransform()"_fmt((int)halign));
        break;
    }

    //--------------------------------------------------
    // 位置(上)を決定
    VerticalAlignment valign = m_verticalAlignment.has_value() ? m_verticalAlignment.value() : VerticalAlignment::Stretch;
    switch (valign)
    {
    case s3d::SamlUI::VerticalAlignment::Bottom:
        m_currentPos.y = parentPos.y + parentSize.y - margin.w - m_currentSize.y;
        break;
    case s3d::SamlUI::VerticalAlignment::Center:
        m_currentPos.y = parentPos.y + parentSize.y / 2 - m_currentSize.y / 2;
        break;
    case s3d::SamlUI::VerticalAlignment::Top:
    case s3d::SamlUI::VerticalAlignment::Stretch:
        m_currentPos.y = parentPos.y + margin.y;
        break;
    default:
        throw Error(U"未定義のHorizontalAlignment \"{}\" @RectElement::updateTransform()"_fmt((int)halign));
        break;
    }

    m_isTransformDirty = false;
}

void UIElement::setMargin(const Optional<Vec4>& margin)
{
    m_margin = margin;
    setTransformDirtyRecursively();
}

void UIElement::setSize(const Optional<Vec2>& size)
{
    m_size = size;
    setTransformDirtyRecursively();
}

void UIElement::setHorizontalAlignment(const Optional<HorizontalAlignment>& alignment)
{
    m_horizontalAlignment = alignment;
    setTransformDirtyRecursively();
}

void UIElement::setVerticalAlignment(const Optional<VerticalAlignment>& alignment)
{
    m_verticalAlignment = alignment;
    setTransformDirtyRecursively();
}