#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;


SamlUI::TextBox::TextPoitionIndexer::TextPoitionIndexer(Vec2 pos, const String& text, const Font& font) :
    m_text(text),
    m_font(font),
    m_pos(pos),
    m_index(0),
    m_height(m_font(' ').region().h),
    m_currentPos(pos),
    m_currenMaxtHeight(-1.0)
{

}

void SamlUI::TextBox::TextPoitionIndexer::next()
{
    const String::value_type& c = m_text.at(m_index);

    if (c == U'\n') 
    {
        m_currentPos.y += m_currenMaxtHeight > 0 ? m_currenMaxtHeight : m_height;
        m_currentPos.x = m_pos.x;
    }
    else 
    {
        RectF charRect = m_font(c).region(m_currentPos);
        m_currentPos.x = charRect.br().x;

        m_currenMaxtHeight = Max(m_currenMaxtHeight, m_height);
    }

    m_index++;
}

SamlUI::TextBox::TextBox() :
    m_font(20),
    m_text(U"aiueoaaaaaaaaaaaaa\nhogehogehoge\bbbbbbbb"),
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
    // キーボードからテキストを入力
    m_cursorPos = TextInput::UpdateText(m_text, m_cursorPos);

    if (KeyRight.down()) { m_cursorPos++; }
    else if (KeyLeft.down()) { m_cursorPos--; }

    RectF rect{ getPosition(), getSize() };

    rect.draw(Palette::Aqua);

    for (TextPoitionIndexer indexer{ getPosition(), m_text, m_font }; ; indexer.next())
    {
        const Vec2& pos = indexer.getPos();

        // 文字の描画
        if (indexer.isValid()) {
            const String::value_type& c = indexer.getChar();
            m_font(c).draw(pos, Palette::Black);
        }

        // カーソルの描画
        size_t index = indexer.getIndex();
        if (index == m_cursorPos) {
            double h = indexer.getFont()(U" ").region().h;
            Line(pos, pos + Vec2{ 0, h }).draw(Palette::Black);
        }

        if (!indexer.isValid()) {
            break;
        }
    }

    //Vec2 pos{ getPosition() };
    //double h = m_font(U' ').region().h;
    //int index = 0;
    //for (auto* c = m_text.c_str(); *c != (String::value_type)0; c++)
    //{
    //    if (index == m_cursorPos) {
    //        Line(pos, pos + Vec2{ 0, h }).draw(Palette::Black);
    //    }

    //    if (*c == U'\n') {
    //        pos.y += h;
    //        pos.x = rect.pos.x;
    //    }
    //    else {
    //        //RectF charRect = m_font(*c).draw(pos, Palette::Black);
    //        m_font(*c).draw(pos, Palette::Black);
    //        RectF charRect = m_font(*c).region(pos);
    //        pos.x = charRect.br().x;
    //        h = Max(h, charRect.h);
    //    }
    //    index++;
    //}

    if (m_isFocused) {
        rect.drawFrame(1, 0, Palette::Black);
    }

    return rect.mouseOver();
}

void SamlUI::TextBox::onClicked()
{
    Vec2 mousePos{ Cursor::Pos() };
    Vec2 charPos{ getPosition() };

    // クリックした箇所が文字列よりも左か上
    if (mousePos.x < charPos.x || mousePos.y < charPos.y) {
        return;
    }

    double h = m_font(U' ').region().h;
    int index = 0;
    for (auto* c = m_text.c_str(); *c != (String::value_type)0; c++)
    {
        if (*c == U'\n') {
            charPos.y += h;
            charPos.x = getPosition().x;
        }
        else {
            RectF charRect = m_font(*c).region(charPos);
            Vec2 charBr = charRect.br();
            charPos.x = charBr.x;
            h = Max(h, charRect.h);

            if (mousePos.x >= charRect.bl().x && mousePos.x <= charBr.x && mousePos.y <= charBr.y) {
                if (mousePos.x <= charRect.center().x) {
                    m_cursorPos = index;
                }
                else {
                    m_cursorPos = index + 1;
                }
                break;
            }
        }
        index++;
    }
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