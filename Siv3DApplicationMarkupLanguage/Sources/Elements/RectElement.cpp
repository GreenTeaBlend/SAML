#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

RectElement::RectElement() :
    m_position(0, 0),
    m_size(200, 50)
{

}

void RectElement::SetProperty(const String& name, const String& value)
{
    if (name == U"Position") {
        m_position = Parse<Vec2>(value);
        return;
    }

    if (name == U"Size") {
        m_size = Parse<Vec2>(value);
        return;
    }

    UIElement::SetProperty(name, value);
}

void RectElement::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{

    //datas->insert(std::make_pair(U"Size",
    //    [&](const String& name, const String& value) {
    //        m_size = Parse<Vec2>(value);
    //    }));
    datas->insert(std::make_pair(U"Position",
        [&](UIElement* elm, const String& name, const String& value) {
            ((RectElement*)elm)->m_position = Parse<Vec2>(value);
        }));

    UIElement::enumratePropertyData(datas);
}

void RectElement::draw()
{
    RectF(m_position, m_size).draw(Palette::Aqua);
}