#include "Elements/RectElement.h"
#include "UIPanel.h"

using namespace s3d;
using namespace SamlUI;

RectElement::RectElement(UIPanel& panel) :
    UIElement(panel),
    m_margin(),
    m_size(),
    m_currentPos(),
    m_currentSize(),
    m_horizontalAlignment(),
    m_verticalAlignment(),
    m_parent()
{

}

RectElement::~RectElement()
{

}

void RectElement::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"Margin",
        [&](UIElement* elm, const String& value) {
            ((RectElement*)elm)->setMargin(Parse<Vec4>(value));
        }));

    datas->insert(std::make_pair(U"Size",
        [&](UIElement* elm, const String& value) {
            ((RectElement*)elm)->setSize(Parse<Vec2>(value));
        }));

    datas->insert(std::make_pair(U"HorizontalAlignment",
        [&](UIElement* elm, const String& value) {
            auto alignment = StringToHorizontalAlignment(value);
            ((RectElement*)elm)->setHorizontalAlignment(alignment);
        }));

    datas->insert(std::make_pair(U"VerticalAlignment",
        [&](UIElement* elm, const String& value) {
            auto alignment = StringToVerticalAlignment(value);
            ((RectElement*)elm)->setVerticalAlignment(alignment);
        }));

    UIElement::enumratePropertyData(datas);
}

void RectElement::updateTransform()
{
    UIElement::updateTransform();

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
}

void RectElement::setParent(const std::shared_ptr<UIElement>& parent)
{
    UIElement::setParent(parent);

    auto rectParent = std::dynamic_pointer_cast<RectElement>(parent);
    if (rectParent != nullptr) {
        m_parent = rectParent;
    }
}

void RectElement::setMargin(const Optional<Vec4>& margin) 
{
    m_margin = margin;
    setTransformDirtyRecursively();
}

void RectElement::setSize(const Optional<Vec2>& size)
{
    m_size = size;
    setTransformDirtyRecursively();
}

void RectElement::setHorizontalAlignment(const Optional<HorizontalAlignment>& alignment)
{
    m_horizontalAlignment = alignment;
    setTransformDirtyRecursively();
}

void RectElement::setVerticalAlignment(const Optional<VerticalAlignment>& alignment)
{
    m_verticalAlignment = alignment;
    setTransformDirtyRecursively();
}