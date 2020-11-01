#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

Button::Button() :
    m_text()
{

}

void Button::SetProperty(const String& name, const String& value) 
{
    if (name == U"Text") {
        m_text = value;
        return;
    }

    RectElement::SetProperty(name, value);
}

void Button::draw()
{
    auto position = 
    RectF(getPosition(), getSize()).draw(Palette::Aqua);
}