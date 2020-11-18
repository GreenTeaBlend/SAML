#include "UIElement.h"
#include "UIElementTypeInfo.h"
#include "Elements/Panel.h"
#include "Elements/Button.h"
#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

UIElement::UIElement():
    m_parent(),
    m_margin(),
    m_width(),
    m_height(),
    m_onTransformDirtyEvents(),
    m_currentPos(),
    m_currentSize(),
    m_horizontalAlignment(HorizontalAlignment::Stretch),
    m_verticalAlignment(VerticalAlignment::Stretch),
    m_isTransformDirty(true)
{
}

UIElement::~UIElement() 
{
    if (m_parent != nullptr) {
        m_parent->removeChild(this);
    }
}

void UIElement::setTransformDirty()
{
    if (m_isTransformDirty) {
        return;
    }

    m_isTransformDirty = true;

    Panel* panel = dynamic_cast<Panel*>(this);
    if (panel != nullptr) {
        for (auto& child : panel->getChildren()) {
            child->setTransformDirty();
        }
    }
}

void UIElement::updateTransform()
{
    RectF parentRect = m_parent != nullptr ? m_parent->getInnerRect() : RectF{ 0, 0, Window::ClientSize() };

    //--------------------------------------------------
    // 幅を決定
    switch (m_horizontalAlignment)
    {
    case HorizontalAlignment::Left:
    case HorizontalAlignment::Right:
    case HorizontalAlignment::Center:
        if (m_width.has_value())
            m_currentSize.x = m_width.value();
        else
            m_currentSize.x = defaultSize().x;
        break;

    case HorizontalAlignment::Stretch:
        m_currentSize.x = parentRect.w - m_margin.x - m_margin.z;
        break;

    default:
        throw Error(U"未定義のHorizontalAlignment \"{}\" @UIElement::updateTransform()"_fmt((int)m_horizontalAlignment));
        break;
    }

    //--------------------------------------------------
    // 高さを決定
    switch (m_verticalAlignment)
    {
    case VerticalAlignment::Top:
    case VerticalAlignment::Bottom:
    case VerticalAlignment::Center:
        if (m_height.has_value())
            m_currentSize.y = m_height.value();
        else
            m_currentSize.y = defaultSize().y;
        break;

    case VerticalAlignment::Stretch:
        m_currentSize.y = parentRect.h - m_margin.y - m_margin.w;
        break;

    default:
        throw Error(U"未定義のVerticalAlignment \"{}\" @UIElement::updateTransform()"_fmt((int)m_verticalAlignment));
        break;
    }

    //--------------------------------------------------
    // 位置(左)を決定
    switch (m_horizontalAlignment)
    {
    case s3d::SamlUI::HorizontalAlignment::Right:
        m_currentPos.x = parentRect.x + parentRect.w - m_margin.z - m_currentSize.x;
        break;
    case s3d::SamlUI::HorizontalAlignment::Center:
        m_currentPos.x = parentRect.x + parentRect.w / 2 - m_currentSize.x / 2;
        break;
    case s3d::SamlUI::HorizontalAlignment::Left:
    case s3d::SamlUI::HorizontalAlignment::Stretch:
        m_currentPos.x = parentRect.x + m_margin.x;
        break;
    default:
        break;
    }

    //--------------------------------------------------
    // 位置(上)を決定
    switch (m_verticalAlignment)
    {
    case s3d::SamlUI::VerticalAlignment::Bottom:
        m_currentPos.y = parentRect.y + parentRect.h - m_margin.w - m_currentSize.y;
        break;
    case s3d::SamlUI::VerticalAlignment::Center:
        m_currentPos.y = parentRect.y + parentRect.h / 2 - m_currentSize.y / 2;
        break;
    case s3d::SamlUI::VerticalAlignment::Top:
    case s3d::SamlUI::VerticalAlignment::Stretch:
        m_currentPos.y = parentRect.y + m_margin.y;
        break;
    default:
        break;
    }

    m_isTransformDirty = false;
}