#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;


SamlUI::TextBox::TextPoitionIndexer::TextPoitionIndexer(Vec2 pos, const String& text, const Font& font) :
    m_text(text),
    m_font(font),
    m_pos(pos),
    m_index(0),
    m_height(m_font(' ').region().h),
    m_currentRegion(),
    m_currenMaxHeight(-1.0)
{

}

void SamlUI::TextBox::TextPoitionIndexer::next()
{
    const String::value_type& c = m_text.at(m_index);

    if (c == U'\n') 
    {
        m_currentRegion.y += m_currenMaxHeight > 0 ? m_currenMaxHeight : m_height;
        m_currentRegion.x = m_pos.x;
    }
    else 
    {
        RectF charRect = m_font(c).region(m_currentRegion.pos);
        m_currentRegion.x = charRect.br().x;

        m_currenMaxHeight = Max(m_currenMaxHeight, m_height);
    }

    // 番号を進める
    m_index++;

    // 次に描画する文字の領域を取得
    if (m_index < m_text.size()) {
        const String::value_type& cNext = m_text.at(m_index);
        m_currentRegion.size = m_font(cNext).region().size;
    }
}

SamlUI::TextBox::TextBox() :
    m_font(20),
    m_text(U""),
    m_isFocused(),
    m_cursorPos(3)
{

}

void SamlUI::TextBox::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"VerticalScrollBarVisibility",
        [&](UIElement* elm, const String& value) {
            ScrollBarVisibility visibility = StringToScrollVarVisibility(value);
            ((SamlUI::TextBox*)elm)->setVerticalScrollBarVisibility(visibility);
        }));

    datas->insert(std::make_pair(U"HorizontalScrollBarVisibility",
        [&](UIElement* elm, const String& value) {
            ScrollBarVisibility visibility = StringToScrollVarVisibility(value);
            ((SamlUI::TextBox*)elm)->setHorizontalScrollBarVisibility(visibility);
        }));

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

    RasterizerState rstatePre = Graphics2D::GetRasterizerState();
    RasterizerState rstateScissor = rstatePre;
    rstateScissor.scissorEnable = true;
    Graphics2D::Internal::SetRasterizerState(rstateScissor);
    Graphics2D::SetScissorRect(rect);

    for (TextPoitionIndexer indexer{ getPosition(), m_text, m_font }; ; indexer.next())
    {
        const RectF& region = indexer.getRegion();

        // 文字の描画
        if (indexer.isValid()) {
            const String::value_type& c = indexer.getChar();
            m_font(c).draw(region.pos, Palette::Black);
        }

        // カーソルの描画
        size_t index = indexer.getIndex();
        if (index == m_cursorPos) {
            Line(region.pos, region.pos + Vec2{ 0, region.h }).draw(Palette::Black);
        }

        if (!indexer.isValid()) {
            break;
        }
    }

    Graphics2D::Internal::SetRasterizerState(rstatePre);

    if (m_isFocused) {
        rect.drawFrame(1, 0, Palette::Black);
    }

    return rect.mouseOver();
}

void SamlUI::TextBox::previewScrollBar(RectF* rect, bool* horizontal, bool* vertical)
{
    ////--------------------------------------------------
    //// 縦方向のスクロールバーの表示判定
    //for (TextPoitionIndexer indexer{ getPosition(), m_text, m_font }; indexer.isValid(); indexer.next())
    //{
    //    const Vec2& pos = indexer.getPos();

    //    // 文字の描画
    //    if (indexer.isValid()) {
    //        const String::value_type& c = indexer.getChar();
    //        m_font(c).draw(pos, Palette::Black);
    //    }

    //    // カーソルの描画
    //    size_t index = indexer.getIndex();
    //    if (index == m_cursorPos) {
    //        double h = indexer.getFont()(U" ").region().h;
    //        Line(pos, pos + Vec2{ 0, h }).draw(Palette::Black);
    //    }

    //    if (!indexer.isValid()) {
    //        break;
    //    }
    //}
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

void SamlUI::TextBox::onMouseOvering() 
{
    Cursor::RequestStyle(s3d::CursorStyle::IBeam);
}