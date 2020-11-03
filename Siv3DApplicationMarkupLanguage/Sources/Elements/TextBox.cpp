#include "Elements/TextBox.h"

using namespace s3d;
using namespace SamlUI;

namespace {
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
    m_scrollView(new ScrollView())
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

    m_scrollView->draw([&]() {

        double widthMax = 0;
        double heightMax = 0;

        Vec2 textTL = TEXT_PADDING;

        // 1文字ずつ描画
        for (TextPoitionIndexer indexer{ textTL, m_text, m_font }; ; indexer.next())
        {
            // これから描画する文字の領域
            const RectF& region = indexer.getRegion();

            // 文字の描画
            // カーソルの描画のために、文字列が終わった後も1文字分だけループを回す。
            if (indexer.isValid()) {
                const String::value_type& c = indexer.getChar();
                if (c != U'\n' && c != U'\r') {
                    m_font(c).draw(region.pos, Palette::Black);
                    widthMax = Max(widthMax, (region.br().x - textTL.x));
                    heightMax = Max(heightMax, (region.br().y - textTL.y));
                }
                else if (c == U'\n') {
                    // 最後に改行文字で終了したときのために、改行後の文字の下端の座標をheightMaxに設定。
                    double y = region.y + indexer.getCurrenMaxHeight() + m_font(U' ').region().h;
                    heightMax = (y - textTL.y);
                }
            }

            // カーソルの描画
            size_t index = indexer.getIndex();
            if (index == m_cursorPos) {
                double h = m_font(U' ').region().h;
                Line(region.pos, region.pos + Vec2{ 0, h }).draw(Palette::Black);
            }

            if (!indexer.isValid()) {
                break;
            }
        }

        return SizeF{ widthMax, heightMax };
        });

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

void SamlUI::TextBox::setCursorPos(size_t pos, bool moveView)
{
    m_cursorPos = Clamp(pos, size_t(0), m_text.size());

    if (moveView)
    {
        Vec2 textTL = TEXT_PADDING;

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
        m_scrollView->moveToShow(regionToShow);
    }
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