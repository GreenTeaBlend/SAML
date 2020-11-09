#include "Elements/RectElement.h"
#include "EventListener.h"

using namespace s3d;
using namespace SamlUI;

RectElement::RectElement(UIPanel& panel) :
    UIElement(panel),
    m_pos(0, 0),
    m_size(50, 50),
    m_isPosDirty(true),
    m_transformChangedEvent(new Event<>())
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

    UIElement::enumratePropertyData(datas);
}

void RectElement::setPosDirtyRecursively()
{
    m_isPosDirty = true;
    for (auto child : getChildren()) {
        RectElement* rectElm = dynamic_cast<RectElement*>(child);
        if (rectElm != nullptr && !rectElm->m_isPosDirty) {
            rectElm->setPosDirtyRecursively();
        }
    }
}

const Vec2& RectElement::getPosition() {
    if (m_isPosDirty) {
        if (getParent() != nullptr) {
            RectElement* rectElm = dynamic_cast<RectElement*>(getParent().get());
            m_pos = rectElm->getPosition() + m_margin.xy();
        }
        else {
            m_pos = m_margin.xy();
        }
        m_isPosDirty = false;
    }

    return m_pos;
}

void RectElement::invokeTransformChangedEvent()
{
    m_transformChangedEvent->invoke();
}