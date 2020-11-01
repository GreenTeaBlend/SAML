#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

SamlUI::TextBox::TextBox()
{

}

void SamlUI::TextBox::SetProperty(const String& name, const String& value)
{
    RectElement::SetProperty(name, value);
}

void SamlUI::TextBox::draw()
{
    RectF(getPosition(), getSize()).draw(Palette::Aqua);
}