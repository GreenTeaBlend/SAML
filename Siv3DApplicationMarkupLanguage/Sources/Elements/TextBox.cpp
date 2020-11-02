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
    // 次に描画する文字の領域を取得
    if (m_index < m_text.size()) {
        const String::value_type& cNext = m_text.at(m_index);
        m_currentRegion.size = m_font(cNext).region().size;
    }
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
    m_cursorPos(3),
    m_verticalBarThickness(20.0),
    m_horizontalBarThickness(20.0)
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

    int cursorPos = static_cast<int>(m_cursorPos);
    if (KeyRight.down()) { cursorPos++; }
    else if (KeyLeft.down()) { cursorPos--; }
    m_cursorPos = static_cast<size_t>(Clamp(cursorPos, 0, (int)m_text.size()));

    RectF rect{ getPosition(), getSize() };

    //--------------------------------------------------
    // 背景の描画
    rect.draw(Palette::White);

    //--------------------------------------------------
    // スクロールバーの描画判定
    ScrollBarState horizontal, vertical;
    previewScrollBar(&horizontal, &vertical);

    //--------------------------------------------------
    // 文字の描画
    RectF textRegion{ rect.pos, rect.w - (vertical.visible ? m_verticalBarThickness : 0), rect.h - (horizontal.visible ? m_horizontalBarThickness : 0) };

    // ステンシル矩形の設定
    RasterizerState rstatePre = Graphics2D::GetRasterizerState();
    RasterizerState rstateScissor = rstatePre;
    rstateScissor.scissorEnable = true;
    Graphics2D::Internal::SetRasterizerState(rstateScissor);

    double widthMax = 0;
    double heightMax = 0;

    // 1文字ずつ描画
    for (TextPoitionIndexer indexer{ textRegion.pos, m_text, m_font }; ; indexer.next())
    {
        // これから描画する文字の領域
        const RectF& region = indexer.getRegion();

        // 文字の描画
        // カーソルの描画のために、文字列が終わった後も1文字分だけループを回す。
        if (indexer.isValid()) {
            const String::value_type& c = indexer.getChar();
            if (c != U'\n' && c != U'\r') {
                m_font(c).draw(region.pos, Palette::Black);
                widthMax = Max(widthMax, region.br().x);
                heightMax = Max(heightMax, region.br().y);
            }
            else if (c == U'\n') {
                // 最後に改行文字で終了したときのために、改行後の文字の下端の座標をheightMaxに設定。
                double y = region.y + indexer.getCurrenMaxHeight() + m_font(U' ').region().h;
                heightMax = y;
            }
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

    // 文字領域描画(デバッグ用)
    //RectF(rect.pos, widthMax, heightMax).draw(ColorF(Palette::Green, 0.3));

    if (widthMax > 0.0001) {
        horizontal.length = (widthMax < rect.w) ? 1.0 : (rect.w - m_verticalBarThickness) / widthMax;
        horizontal.pos = 0.0;
    }
    if (heightMax > 0.0001) {
        vertical.length = (heightMax < rect.h) ? 1.0 : (rect.h - m_horizontalBarThickness) / heightMax;
        vertical.pos = 0.0;
    }

    // ステンシル矩形の設定を戻す
    Graphics2D::SetScissorRect(textRegion);
    Graphics2D::Internal::SetRasterizerState(rstatePre);

    // スクロールバーの描画
    drawScrollBar(horizontal, vertical);

    //--------------------------------------------------
    // 枠線の描画
    if (m_isFocused) {
        rect.drawFrame(1, 2, Palette::Aqua);
    }
    else {
        rect.drawFrame(1, 0, Palette::Gray);
    }

    return rect.mouseOver();
}

void SamlUI::TextBox::previewScrollBar(ScrollBarState* horizontal, ScrollBarState* vertical)
{
    horizontal->visible = true;
    horizontal->length = 0.5;
    horizontal->pos = 0.25;

    vertical->visible = true;
    vertical->length = 0.5;
    vertical->pos = 0.25;

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

void SamlUI::TextBox::drawScrollBar(const ScrollBarState& horizontal, const ScrollBarState& vertical)
{
    RectF rect{ getPosition(), getSize() };
    static const double PAD = 4.0;
    static const double ROUND = 2.0;

    //--------------------------------------------------
    // 垂直方向のバー
    Vec2 vbarPos = rect.tr() - Vec2{ m_verticalBarThickness, 0 };
    Vec2 vbarSize = Vec2(m_verticalBarThickness, rect.h - (horizontal.visible ? m_horizontalBarThickness : 0));
    RectF(vbarPos, vbarSize).draw(Palette::Wheat);

    // 内側のバー
    RectF(vbarPos + PAD * Vec2::One() + Vec2{ 0, vertical.pos * (vbarSize.y - 2 * PAD) },
        vbarSize.x - 2 * PAD,
        vertical.length * (vbarSize.y - 2 * PAD))
        .rounded(ROUND)
        .draw(Palette::Gray);

    //--------------------------------------------------
    // 水平方向のバー
    Vec2 hbarPos = rect.bl() - Vec2{ 0, m_horizontalBarThickness };
    Vec2 hbarSize = Vec2(rect.w - (vertical.visible ? m_verticalBarThickness : 0), m_horizontalBarThickness);
    RectF(hbarPos, hbarSize).draw(Palette::Wheat);

    // 内側のバー
    RectF(hbarPos + PAD * Vec2::One() + Vec2{ horizontal.pos * (hbarSize.x - 2 * PAD), 0 },
        horizontal.length * (hbarSize.x - 2 * PAD),
        hbarSize.y - 2 * PAD)
        .rounded(ROUND)
        .draw(Palette::Gray);
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