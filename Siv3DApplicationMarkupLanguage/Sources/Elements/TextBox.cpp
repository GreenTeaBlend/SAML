#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

SamlUI::TextBox::TextBox() :
    m_font(20),
    m_text(U"aiueoaaaaaaaaaaaaaaaaaaaa"),
    m_isFocused(),
    m_cursorPos(3)
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

bool SamlUI::TextBox::draw()
{
    RectF rect{ getPosition(), getSize() };

    rect.draw(Palette::Aqua);

    String text = U"aiueoauieo\nhogehogehogehoge\nhogehogehogehoge\nhogehogehogehoge";
    //m_font(text).draw(rect.pos, Palette::Black);

    Vec2 pos{ rect.pos };
    double h = m_font(U' ').region().h;
    int index = 0;
    for (auto* c = text.c_str(); *c != (String::value_type)0; c++) 
    {
        if (index == m_cursorPos) {
            Line(pos, pos + Vec2{ 0, h }).draw(Palette::Black);
        }

        if (*c == U'\n') {
            pos.y += h;
            pos.x = rect.pos.x;
        }
        else {
            RectF charRect = m_font(*c).draw(pos, Palette::Black);
            pos.x = charRect.br().x;
            h = Max(h, charRect.h);
        }
        index++;
    }

    if (m_isFocused) {
        rect.drawFrame(1, 0, Palette::Black);
    }

    return rect.mouseOver();
}

void SamlUI::TextBox::onFocusStart()
{
    m_isFocused = true;
}

void SamlUI::TextBox::onFocusEnd()
{
    m_isFocused = false;
}

void SamlUI::TextBox::onMouseOverStart()
{
    Cursor::SetDefaultStyle(s3d::CursorStyle::IBeam);
}

void SamlUI::TextBox::onMouseOverEnd()
{
    Cursor::SetDefaultStyle(s3d::CursorStyle::Default);
}