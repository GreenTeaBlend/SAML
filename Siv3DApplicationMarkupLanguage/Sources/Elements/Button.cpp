#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

Button::Button() :
    m_text()
{

}

void Button::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"Text",
        [&](UIElement* elm, const String& value) {
            ((Button*)elm)->m_text = value;
        }));

    RectElement::enumratePropertyData(datas);
}

bool Button::draw()
{
    RectF rect{ getPosition(), getSize() };
    rect.draw(Palette::Aqua);

    return rect.mouseOver();
}