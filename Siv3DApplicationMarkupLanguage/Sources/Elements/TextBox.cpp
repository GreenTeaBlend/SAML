#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    const double WHEEL_SPEED = 15.0;
    const Vec2 TEXT_PADDING{ 3, 0 };
}

SamlUI::TextBox::TextPoitionIndexer::TextPoitionIndexer(Vec2 pos, const String& text, const Font& font) :
    m_text(text),
    m_font(font),
    m_tlPos(pos),
    m_index(0),
    m_height(m_font(' ').region().h),
    m_currentRegion(pos, 0, 0),
    m_currenMaxHeight(-1.0)
{
    // 次に描画する文字の領域を取得
    const String::value_type& cNext = m_index < m_text.size() ? m_text.at(m_index) : U' ';
    m_currentRegion.size = m_font(cNext).region().size;
}

void SamlUI::TextBox::TextPoitionIndexer::next()
{
    const String::value_type& c = m_text.at(m_index);

    if (c == U'\n') 
    {
        m_currentRegion.y += m_currenMaxHeight > 0 ? m_currenMaxHeight : m_height;
        m_currentRegion.x = m_tlPos.x;
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
    const String::value_type& cNext = m_index < m_text.size() ? m_text.at(m_index) : U' ';
    m_currentRegion.size = m_font(cNext).region().size;
}

SamlUI::TextBox::TextBox() :
    m_font(20),
    m_text(U""),
    m_isFocused(),
    m_cursorPos(3),
    m_horizontalBarThickness(20.0),
    m_verticalBarThickness(20.0),
    m_horizontalBarState(),
    m_verticalBarState()
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
    // 文字の描画

    // ステンシル矩形の設定
    RasterizerState rstatePre = Graphics2D::GetRasterizerState();
    RasterizerState rstateScissor = rstatePre;
    rstateScissor.scissorEnable = true;
    Graphics2D::Internal::SetRasterizerState(rstateScissor);

    double widthMax = 0;
    double heightMax = 0;

    // 1文字ずつ描画
    Vec2 textTLPos = getTextTL();
    for (TextPoitionIndexer indexer{ textTLPos, m_text, m_font }; ; indexer.next())
    {
        // これから描画する文字の領域
        const RectF& region = indexer.getRegion();

        // 文字の描画
        // カーソルの描画のために、文字列が終わった後も1文字分だけループを回す。
        if (indexer.isValid()) {
            const String::value_type& c = indexer.getChar();
            if (c != U'\n' && c != U'\r') {
                m_font(c).draw(region.pos, Palette::Black);
                widthMax = Max(widthMax, (region.br().x - textTLPos.x));
                heightMax = Max(heightMax, (region.br().y - textTLPos.y));
            }
            else if (c == U'\n') {
                // 最後に改行文字で終了したときのために、改行後の文字の下端の座標をheightMaxに設定。
                double y = region.y + indexer.getCurrenMaxHeight() + m_font(U' ').region().h;
                heightMax = (y - textTLPos.y);
            }
        }

        // カーソルの描画
        size_t index = indexer.getIndex();
        if (index == m_cursorPos) {
            double h = m_font(U' ').region().h;
            Line(region.pos, region.pos + Vec2{ 0, region.h }).draw(Palette::Black);
        }

        if (!indexer.isValid()) {
            break;
        }
    }

    // 文字領域描画(デバッグ用)
    //RectF(rect.pos, widthMax, heightMax).draw(ColorF(Palette::Green, 0.3));

    // スクロールバーの描画判定と、テキスト描画領域の計算
    m_horizontalBarState.visible = true;
    m_verticalBarState.visible = true;
    RectF textRegion{ rect.pos, rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0) };

    // ステンシル矩形の設定を戻す
    Graphics2D::SetScissorRect(textRegion);
    Graphics2D::Internal::SetRasterizerState(rstatePre);

    if (textRegion.w >= 0.0001 && textRegion.h >= 0.0001)
    {
        m_horizontalBarState.actualSize = widthMax;
        m_verticalBarState.actualSize = heightMax;

        m_horizontalBarState.length = widthMax > 1.0 ? Min(textRegion.w / widthMax, 1.0) : 1.0;
        m_verticalBarState.length = heightMax > 1.0 ? Min(textRegion.h / heightMax, 1.0) : 1.0;

        double wheel = WHEEL_SPEED * Mouse::Wheel();
        m_verticalBarState.pos += wheel / m_verticalBarState.actualSize;
        m_verticalBarState.pos = Clamp(m_verticalBarState.pos, 0.0, 1.0 - m_verticalBarState.length);
    }

    // スクロールバーの描画
    drawScrollBar();

    if (KeyHome.down()) setCursorPos(0, true);
    if (KeyEnd.down()) setCursorPos(m_text.size(), true);

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

void SamlUI::TextBox::drawScrollBar()
{
    RectF rect{ getPosition(), getSize() };
    static const double PAD = 4.0;
    static const double ROUND = 2.0;

    //--------------------------------------------------
    // 垂直方向のバー
    Vec2 vbarPos = rect.tr() - Vec2{ m_verticalBarThickness, 0 };
    Vec2 vbarSize = Vec2(m_verticalBarThickness, rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0));
    RectF(vbarPos, vbarSize).draw(Palette::Wheat);

    // 内側のバー
    RectF(vbarPos + PAD * Vec2::One() + Vec2{ 0, m_verticalBarState.pos * (vbarSize.y - 2 * PAD) },
        vbarSize.x - 2 * PAD,
        m_verticalBarState.length * (vbarSize.y - 2 * PAD))
        .rounded(ROUND)
        .draw(Palette::Gray);

    //--------------------------------------------------
    // 水平方向のバー
    Vec2 hbarPos = rect.bl() - Vec2{ 0, m_horizontalBarThickness };
    Vec2 hbarSize = Vec2(rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), m_horizontalBarThickness);
    RectF(hbarPos, hbarSize).draw(Palette::Wheat);

    // 内側のバー
    RectF(hbarPos + PAD * Vec2::One() + Vec2{ m_horizontalBarState.pos * (hbarSize.x - 2 * PAD), 0 },
        m_horizontalBarState.length * (hbarSize.x - 2 * PAD),
        hbarSize.y - 2 * PAD)
        .rounded(ROUND)
        .draw(Palette::Gray);
}

void SamlUI::TextBox::setCursorPos(size_t pos, bool moveView)
{
    m_cursorPos = Clamp(pos, size_t(0), m_text.size());

    if (moveView && m_horizontalBarState.actualSize >= 0.0001 && m_verticalBarState.actualSize >= 0.0001)
    {
        Vec2 textTL = getTextTL();
        RectF regionToShow;
        for (TextPoitionIndexer indexer{ textTL, m_text, m_font }; ; indexer.next())
        {
            if (indexer.getIndex() == pos) {
                regionToShow = indexer.getRegion();
                break;
            }

            // m_cursorPosは文字列の外にセットされることはないので、このreturnは通らないはず。
            if (!indexer.isValid()) {
                return;
            }
        }

        // テキストの描画開始点からの相対位置
        regionToShow.moveBy(-textTL);

        //--------------------------------------------------
        // regionToShowが表示領域に入るようにスクロールする。

        // regionToShowの左側が見えるようになるためのバーの位置の最小値。以下、右上下についても同様。
        double leftRelativePos = regionToShow.x / m_horizontalBarState.actualSize;
        if (m_horizontalBarState.pos > leftRelativePos) { m_horizontalBarState.pos = leftRelativePos; }

        double rightRelativePos = (regionToShow.x + regionToShow.w) / m_horizontalBarState.actualSize - m_horizontalBarState.length;
        if (m_horizontalBarState.pos < rightRelativePos) { m_horizontalBarState.pos = rightRelativePos; }

        double topRelativePos = regionToShow.y / m_verticalBarState.actualSize;
        if (m_verticalBarState.pos > topRelativePos) { m_verticalBarState.pos = topRelativePos; }

        double bottomRelativePos = (regionToShow.y + regionToShow.h) / m_verticalBarState.actualSize - m_verticalBarState.length;
        if (m_verticalBarState.pos < bottomRelativePos) { m_verticalBarState.pos = bottomRelativePos; }
    }
}

Vec2 SamlUI::TextBox::getTextTL() const
{
    return getPosition() + TEXT_PADDING - Vec2{ m_horizontalBarState.pos * m_horizontalBarState.actualSize, m_verticalBarState.pos * m_verticalBarState.actualSize };
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