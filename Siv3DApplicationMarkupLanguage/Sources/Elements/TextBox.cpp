#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

SamlUI::TextBox::TextBox() :
    m_position(0, 0),
    m_size(200, 50)
{

}

void SamlUI::TextBox::SetProperty(const String& name, const String& value)
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

void SamlUI::TextBox::draw()
{
    RectF(m_position, m_size).draw(Palette::Aqua);
}