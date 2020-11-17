#include "UIElement.h"
#include "UIElementTypeInfo.h"
#include "Elements/Button.h"
#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

UIElement::UIElement():
    m_parent(),
    m_isMouseOvered(),
    m_margin(),
    m_width(),
    m_height(),
    m_currentPos(),
    m_currentSize(),
    m_horizontalAlignment(HorizontalAlignment::Stretch),
    m_verticalAlignment(VerticalAlignment::Stretch),
    m_isTransformDirty(true)
{
}

UIElement::~UIElement() 
{
    setParent(nullptr);

    if (m_children.size() != 0) {
        auto children = Array<UIElement*>(m_children);
        for (auto* child : children) {
            child->setParent(nullptr);
        }
    }
}

void UIElement::setParent(UIElement* parent)
{
    if (parent != m_parent) 
    {
        // 前のParentのChildrenから自分を削除する。
        if (m_parent != nullptr) 
        {
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
    Vec2 parentPos = m_parent != nullptr ? m_parent->getCurrentPosition() : Vec2::Zero();
    Vec2 parentSize = m_parent != nullptr ? m_parent->getCurrentSize() : Window::ClientSize();

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
            m_currentSize.x = getContentSize().x;
        break;

    case HorizontalAlignment::Stretch:
        m_currentSize.x = parentSize.x - m_margin.x - m_margin.z;
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
            m_currentSize.y = getContentSize().y;
        break;

    case VerticalAlignment::Stretch:
        m_currentSize.y = parentSize.y - m_margin.y - m_margin.w;
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
        m_currentPos.x = parentPos.x + parentSize.x - m_margin.z - m_currentSize.x;
        break;
    case s3d::SamlUI::HorizontalAlignment::Center:
        m_currentPos.x = parentPos.x + parentSize.x / 2 - m_currentSize.x / 2;
        break;
    case s3d::SamlUI::HorizontalAlignment::Left:
    case s3d::SamlUI::HorizontalAlignment::Stretch:
        m_currentPos.x = parentPos.x + m_margin.x;
        break;
    default:
        break;
    }

    //--------------------------------------------------
    // 位置(上)を決定
    switch (m_verticalAlignment)
    {
    case s3d::SamlUI::VerticalAlignment::Bottom:
        m_currentPos.y = parentPos.y + parentSize.y - m_margin.w - m_currentSize.y;
        break;
    case s3d::SamlUI::VerticalAlignment::Center:
        m_currentPos.y = parentPos.y + parentSize.y / 2 - m_currentSize.y / 2;
        break;
    case s3d::SamlUI::VerticalAlignment::Top:
    case s3d::SamlUI::VerticalAlignment::Stretch:
        m_currentPos.y = parentPos.y + m_margin.y;
        break;
    default:
        break;
    }

    m_isTransformDirty = false;
}

void UIElement::draw()
{
    bool mouseOverDisabled = false;

    drawRecursively(mouseOverDisabled);
}

void UIElement::drawRecursively(bool& mouseOverDisabled)
{
    m_isMouseOvered = !mouseOverDisabled && intersectsMouse();
    if (m_isMouseOvered) {
        mouseOverDisabled = true;
    }

    onDraw();

    for (auto* child : m_children) {
        drawRecursively(mouseOverDisabled);
    }
};