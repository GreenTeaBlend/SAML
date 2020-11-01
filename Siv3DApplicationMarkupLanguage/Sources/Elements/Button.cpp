#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

Button::Button() :
    m_position(0, 0),
    m_size(200, 50)
{

}

void Button::SetProperty(const String& name, const String& value) 
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

void Button::draw()
{
    RectF(m_position, m_size).draw(Palette::Aqua);
}