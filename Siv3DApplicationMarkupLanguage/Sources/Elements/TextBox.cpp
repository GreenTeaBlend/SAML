#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

SamlUI::TextBox::TextBox()
{

}

void SamlUI::TextBox::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    //datas->insert(std::make_pair(U"Text",
    //    [&](UIElement* elm, const String& value) {
    //        ((SamlUI::TextBox*)elm)->m_text = value;
    //    }));

    RectElement::enumratePropertyData(datas);
}

void SamlUI::TextBox::draw()
{
    RectF(getPosition(), getSize()).draw(Palette::Aqua);
}