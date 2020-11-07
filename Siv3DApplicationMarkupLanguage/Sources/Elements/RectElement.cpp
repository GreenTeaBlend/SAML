#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

RectElement::RectElement(UIPanel& panel) :
    UIElement(panel),
    m_rect(0, 0, 50, 50)
{

}

void RectElement::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"Position",
        [&](UIElement* elm, const String& value) {
            ((RectElement*)elm)->setPosition(Parse<Vec2>(value));
        }));

    datas->insert(std::make_pair(U"Size",
        [&](UIElement* elm, const String& value) {
            ((RectElement*)elm)->setSize(Parse<Vec2>(value));
        }));

    UIElement::enumratePropertyData(datas);
}