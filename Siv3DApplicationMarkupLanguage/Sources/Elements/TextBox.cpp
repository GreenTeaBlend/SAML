#include "Elements/TextBox.h"
#include "UITextIndexer.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    // 右上左下
    const Vec4 TEXT_PADDING{ 10, 0, 30, 0 };
}

SamlUI::TextBox::TextBox() :
    m_font(20),
    m_text(U""),
    m_lines(),
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
    if (TextInput::GetRawInput().length() != 0) {
        String text = m_text;
        m_cursorPos = TextInput::UpdateText(text, m_cursorPos);
        setText(text);
    }

    // カーソル移動
    int cursorPos = static_cast<int>(m_cursorPos);
    if (KeyRight.down()) { cursorPos++; }
    else if (KeyLeft.down()) { cursorPos--; }
    else if (KeyHome.down()) { cursorPos = 0; }
    else if (KeyEnd.down()) { cursorPos = (int)m_text.size(); }
    cursorPos = static_cast<size_t>(Clamp(cursorPos, 0, (int)m_text.size()));
    if (m_cursorPos != cursorPos) {
        setCursorPos(cursorPos, true);
    }

    // スクロールバーと内側の描画
    m_scrollView->draw([&](bool isMouseOvered) {
        return drawInner(isMouseOvered);
        }, isMouseOvered());

    // 枠線の描画
    RectF rect{ getPosition(), getSize() };
    if (isFocusing()) {
        rect.drawFrame(1, 2, Palette::Aqua);
    }
    else {
        rect.drawFrame(1, 0, Palette::Gray);
    }

    return rect.mouseOver();
}

SizeF SamlUI::TextBox::drawInner(bool isMouseOvered)
{
    double widthMax = 0;
    double heightMax = 0;

    Vec2 textTL = TEXT_PADDING.xy();

    // 1文字ずつ描画
    for (UITextIndexer indexer{ textTL, m_text, m_font }; ; indexer.next())
    {
        // これから描画する文字の領域
        const RectF& region = indexer.currentRegion();

        // 文字の描画
        // カーソルの描画のために、文字列が終わった後も1文字分だけループを回す。
        if (indexer.isValid()) {
            const String::value_type& c = indexer.currentChar();
            if (c != U'\n' && c != U'\r') {
                m_font(c).draw(region.pos, Palette::Black);
                widthMax = Max(widthMax, (region.br().x - textTL.x));
                heightMax = Max(heightMax, (region.br().y - textTL.y));
            }
            else if (c == U'\n') {
                // 最後に改行文字で終了したときのために、改行後の文字の下端の座標をheightMaxに設定。
                double y = region.y + indexer.currentHeight() + m_font(U' ').region().h;
                heightMax = (y - textTL.y);
            }
        }

        // カーソルの描画
        size_t index = indexer.currentIndex();
        if (index == m_cursorPos) {
            double h = m_font(U' ').region().h;
            Line(region.pos, region.pos + Vec2{ 0, h }).draw(Palette::Black);
        }

        if (!indexer.isValid()) {
            break;
        }
    }

    // 行のデバッグ表示
    for (LineInfo& line : m_lines) {
        String text = line.text.removed(U'\n');
        m_font(text).region(textTL + line.offset).draw(ColorF(Palette::Green, 0.3));
    }

    // 内側がマウスオーバーされてたらカーソルの形を変える。
    if (isMouseOvered){
        Cursor::RequestStyle(s3d::CursorStyle::IBeam);
    }

    return SizeF{ widthMax, heightMax } + TEXT_PADDING.zw();
}

void SamlUI::TextBox::setCursorPos(size_t pos, bool moveView)
{
    m_cursorPos = Clamp(pos, size_t(0), m_text.size());

    if (moveView)
    {
        Vec2 textTL = TEXT_PADDING.xy();

        RectF regionToShow;
        for (UITextIndexer indexer{ textTL, m_text, m_font }; ; indexer.next())
        {
            if (indexer.currentIndex() == pos) {
                regionToShow = indexer.currentRegion();
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
    Vec2 textTL = m_scrollView->getRect().pos + m_scrollView->offset() + TEXT_PADDING.xy();

    // 引数の行をクリックしたとして、x座標を参考にクリックした場所を返す関数。
    auto selectedIndexInLine = [&](const Vec2& lineTL, const String& lineText) {
        for (UITextIndexer indexer{ lineTL, lineText, m_font }; indexer.isValid(); indexer.next())
        {
            RectF charRect = indexer.currentRegion();

            if (mousePos.x <= charRect.center().x) {
                return indexer.currentIndex();
            }
        }

        if (lineText.back() == U'\n') {
            return lineText.size() - 1;
        }
        else {
            return lineText.size();
        }
    };

    for (LineInfo& line : m_lines) 
    {
        Vec2 lineTL = (textTL + line.offset);
        if (mousePos.y < lineTL.y + line.height) 
        {
            m_cursorPos = line.index + selectedIndexInLine(lineTL, line.text);
            return;
        }
    }

    // 文字列中の最後の行よりもクリック位置が下側の場合、最後の行の中でx座標のみを参考にカーソル位置を決める
    if (m_text.size() == 0 || m_text.ends_with(U'\n')) {
        m_cursorPos = m_text.size();
    }
    else {
        auto& line = m_lines.back();
        Vec2 lineTL = (textTL + line.offset);
        m_cursorPos = line.index + selectedIndexInLine(lineTL, line.text);
    }
}

void SamlUI::TextBox::setText(const String& text)
{
    m_text = text;
    m_lines.clear();

    LineInfo* currentLine = nullptr;

    for (UITextIndexer indexer{ Vec2::Zero(), m_text, m_font }; ; indexer.next())
    {
        if (indexer.isValid() == false || indexer.currentChar() == U'\n')
        {
            if (currentLine != nullptr) {
                currentLine->text = m_text.substr(currentLine->index, indexer.currentIndex() - currentLine->index + 1);
                currentLine = nullptr;
            }

            if (indexer.isValid() == false) {
                return;
            }
        }
        else
        {
            if(currentLine == nullptr)
            {
                m_lines.push_back(LineInfo());
                currentLine = &m_lines.back();
                currentLine->index = indexer.currentIndex();
                currentLine->offset = indexer.currentRegion().pos;
            }

            currentLine->height = Max(currentLine->height, indexer.currentRegion().h);
        }
    }
}

void SamlUI::TextBox::insertText(const String& text, size_t index)
{

}