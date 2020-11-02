#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

RectElement::RectElement() :
    m_position(0, 0),
    m_size(200, 50)
{

}

void RectElement::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"Position",
        [&](UIElement* elm, const String& value) {
            ((RectElement*)elm)->m_position = Parse<Vec2>(value);
        }));

    datas->insert(std::make_pair(U"Size",
        [&](UIElement* elm, const String& value) {
            ((RectElement*)elm)->m_size = Parse<Vec2>(value);
        }));

    UIElement::enumratePropertyData(datas);
}