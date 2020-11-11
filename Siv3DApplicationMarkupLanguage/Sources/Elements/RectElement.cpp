#include "Elements/RectElement.h"
#include "UIPanel.h"

using namespace s3d;
using namespace SamlUI;

RectElement::RectElement(UIPanel& panel) :
    UIElement(panel),
    m_pos(0, 0),
    m_size(50, 50),
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

    UIElement::enumratePropertyData(datas);
}

void RectElement::updateTransform()
{
    UIElement::updateTransform();

    if (m_parent != nullptr) {
        m_pos = m_parent->getPosition() + m_margin.xy();
        m_size = m_parent->getSize() - m_margin.xy() - m_margin.zw();
    }
    else {
        m_pos = getPanel().getRect().pos + m_margin.xy();
        m_size = getPanel().getRect().size - m_margin.xy() - m_margin.zw();
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

void RectElement::setMargin(const Vec4& margin) {
    m_margin = margin;
    setTransformDirtyRecursively();
}