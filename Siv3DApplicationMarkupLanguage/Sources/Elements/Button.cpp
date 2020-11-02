#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

Button::Button() :
    m_text(U"button"),
    m_font(20)
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
    rect.draw(Palette::White);
    rect.drawFrame(1.0, Palette::Black);

    m_font(m_text).drawAt(rect.center(), Palette::Black);

    return rect.mouseOver();
}