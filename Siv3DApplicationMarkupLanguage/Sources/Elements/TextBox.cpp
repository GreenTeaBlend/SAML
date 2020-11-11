#include "Elements/TextBox.h"
#include "UITextIndexer.h"
#include "EventListener.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    // 右上左下
    const Vec4 TEXT_PADDING{ 6, 0, 30, 0 };

    // キー長押しによるカーソル移動の間隔 (横方向)
    const double KEY_INTERVAL_H{ 0.03 };
    // キー長押しによるカーソル移動の間隔 (縦方向)
    const double KEY_INTERVAL_V{ 0.06 };

    /// <summary>
    /// 引数の行をクリックしたとして、x座標を参考にクリックした場所(行頭の文字を0とした番号)を返す
    /// </summary>
    /// <param name="mouseX">マウス位置x座標</param>
    /// <param name="lineTL">mouseXと同じ座標系での、調べたい行の左上の座標</param>
    size_t calcCursorPosInLine(double mouseX, const Font& font, const Vec2& lineTL, const String& lineText)
    {
        for (UITextIndexer indexer{ lineTL, lineText, font }; indexer.isValid(); indexer.next())
        {
            RectF charRect = indexer.currentRegion();

            if (mouseX <= charRect.center().x) {
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

    /// <summary>
    /// 引数の行をクリックしたとして、x座標を参考にクリックした場所(行頭の文字を0とした番号)を返す
    /// </summary>
    /// <param name="mouseX">マウス位置x座標</param>
    /// <param name="lineTL">mouseXと同じ座標系での、調べたい行の左上の座標</param>
    size_t calcCursorPos(const Vec2& textTL, const Vec2& mousePos, const Font& font, const Array<s3d::SamlUI::TextBox::LineInfo>& lines)
    {
        for (const auto& line : lines)
        {
            Vec2 lineTL = (textTL + line.offset);
            if (mousePos.y < lineTL.y + line.height)
            {
                return line.index + calcCursorPosInLine(mousePos.x, font, lineTL, line.text);
            }
        }

        // 文字列中の最後の行よりもクリック位置が下側の場合、最後の行の中でx座標のみを参考にカーソル位置を決める
        if (lines.size() == 0 || lines.back().text.ends_with(U'\n')) {
            return lines.size() == 0 ? 0 : lines.back().index + lines.back().text.size();
        }
        else {
            auto& line = lines.back();
            Vec2 lineTL = (textTL + line.offset);
            return line.index + calcCursorPosInLine(mousePos.x, font, lineTL, line.text);
        }
    };

    /// <summary>
    /// 文字列内での番号 index について、その文字列の行の集合 lines の何番目に属するかを返す。
    /// </summary>
    size_t getLineIndexOf(size_t index, const Array<SamlUI::TextBox::LineInfo>& lines)
    {
        if (lines.size() == 0) {
            return 0;
        }

        for (size_t i = 0; i < lines.size(); ++i)
        {
            const auto& line = lines[i];
            if (index < line.index + line.text.size()) {
                return i;
            }
        }

        // 最後の行が改行文字で終わっているなら、範囲外のカーソルは最後の行の次の行。
        return lines.back().text.ends_with(U'\n') ? lines.size() : lines.size() - 1;
    }
}

SamlUI::TextBox::TextBox(UIPanel& panel) :
    RectElement(panel),
    m_font(20),
    m_text(U""),
    m_lines(),
    m_cursorPos(3),
    m_scrollView(new ScrollView()),
    m_selectRange(),
    m_keyPressStopwatch(),
    m_cursorStopwatch()
{
    m_keyPressStopwatch.start();
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

void SamlUI::TextBox::draw()
{
    // 文字入力
    updateText();

    // キー入力によるカーソル移動
    updateCursor();

    // クリック・ドラッグなど処理
    updateMouse();

    // スクロールバーの矩形更新
    // TODO:毎フレームsetRectするのはちょっと歪なので、ScrollView::draw()に毎回矩形を渡すようにする
    m_scrollView->setRect(RectF{ getPosition(), getSize() });

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

    return;
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
                widthMax = Max(widthMax, region.br().x);
                heightMax = Max(heightMax, region.br().y);
            }
            else if (c == U'\n') {
                // 最後に改行文字で終了したときのために、改行後の文字の下端の座標をheightMaxに設定。
                double y = region.y + indexer.currentHeight() + m_font(U' ').region().h;
                heightMax = y;
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

    // 選択領域の描画
    drawSelection();

    // 内側がマウスオーバーされてたらカーソルの形を変える。
    if (isMouseOvered){
        Cursor::RequestStyle(s3d::CursorStyle::IBeam);
    }

    return SizeF{ widthMax, heightMax } + TEXT_PADDING.zw();
}

void SamlUI::TextBox::drawSelection()
{
    if (!m_selectRange.has_value()) {
        return;
    }

    m_selectRange->start = Clamp(m_selectRange->start, (size_t)0, m_text.size());
    m_selectRange->current = Clamp(m_selectRange->current, (size_t)0, m_text.size());

    if (m_selectRange->start == m_selectRange->current) {
        return;
    }

    const ColorF color = ColorF(Palette::Green, 0.3);
    const Vec2 textTL = TEXT_PADDING.xy();
    const size_t start = Min(m_selectRange->start, m_selectRange->current);
    const size_t end = Max(m_selectRange->start, m_selectRange->current);

    /// <summary>
    /// 引数の行における選択の矩形を描画する。
    /// </summary>
    /// <param name="start">選択範囲開始番号(m_text内での番号)</param>
    /// <param name="end">選択範囲終了番号(m_text内での番号)</param>
    static const auto drawLineSelection = [&](const LineInfo line)
    {
        String text = line.text/*.removed(U'\n')*/;

        // 末尾が改行文字ならスペースに置換する。
        if (text.ends_with(U'\n')) {
            text = text.replaced(U'\n', U' ');
        }

        if (start >= line.index + text.size() || end <= line.index) {
            return;
        }

        // 矩形の左端の座標
        double left = textTL.x + line.offset.x;
        if (start > line.index) {
            String textSubstr = text.substr(0, start - line.index);
            left += m_font(textSubstr).region().br().x;
        }

        // 矩形の右端の座標
        double right = textTL.x;
        if (end < line.index + text.size()) {
            String textSubstr = text.substr(0, end - line.index);
            right += m_font(textSubstr).region().br().x;
        }
        else {
            right += m_font(text).region().br().x;
        }

        // 描画
        RectF(left, line.offset.y, right - left, line.height).draw(color);
    };

    size_t lineStart = getLineIndexOf(start, m_lines);
    size_t lineEnd = getLineIndexOf(end, m_lines);
    for (size_t i = lineStart; i <= lineEnd && i != m_lines.size(); ++i) {
        const LineInfo& line = m_lines[i];
        drawLineSelection(line);
    }
}

void SamlUI::TextBox::updateText()
{
    // キーボードからテキストを入力
    String inputText = TextInput::GetRawInput();

    if (inputText.length() != 0)
    {
        // 削除文字が入力されたか否か
        bool deleteCharInputed = false;
        for (char32_t c : inputText) {
            if (c == U'\b' || c == (char32_t)127) {
                deleteCharInputed = true;
                break;
            }
        }

        String text = m_text;
        if (deleteCharInputed && m_selectRange.has_value() && m_selectRange->length() != 0) {
            // 削除文字が入力されたら選択範囲を削除する。
            size_t min = Min(m_selectRange->start, m_selectRange->current);
            size_t max = Max(m_selectRange->start, m_selectRange->current);
            text = text.substr(0, min) + text.substr(max);
            m_cursorPos = min;
        }
        else {
            // 通常の文字の挿入
            m_cursorPos = TextInput::UpdateText(text, m_cursorPos);
        }
        setText(text);

        // 範囲選択をしていた場合は解除する。
        if (m_selectRange.has_value()) {
            m_selectRange = Optional<IndexRange>();
        }
    }
}

void SamlUI::TextBox::updateCursor()
{
    size_t cursorPos = m_cursorPos;

    // 左右移動
    if (KeyRight.down() || (KeyRight.pressedDuration() > SecondsF(0.33) && m_keyPressStopwatch > SecondsF(KEY_INTERVAL_H)))
    {
        m_keyPressStopwatch.restart();
        cursorPos++; 
    }
    if (KeyLeft.down() || (KeyLeft.pressedDuration() > SecondsF(0.33) && m_keyPressStopwatch > SecondsF(KEY_INTERVAL_H)))
    {
        m_keyPressStopwatch.restart();
        cursorPos = cursorPos >= 1 ? cursorPos - 1 : 0; 
    }

    // 上の行に移動
    if (KeyUp.down() || (KeyUp.pressedDuration() > SecondsF(0.33) && m_keyPressStopwatch > SecondsF(KEY_INTERVAL_V)))
    {
        m_keyPressStopwatch.restart();

        size_t lineIndex = getLineIndexOf(m_cursorPos, m_lines);
        if (lineIndex != 0) {
            // 現在のカーソルのx座標を取得する。
            double xPos = 0;
            if (lineIndex != m_lines.size()) {
                const auto& lineCurrent = m_lines[lineIndex];
                UITextIndexer indexer{ Vec2::Zero(), lineCurrent.text, m_font };
                indexer.nextUntil(m_cursorPos - lineCurrent.index);
                xPos = indexer.currentRegion().x;
            }

            // 上の行でカーソル位置を取得する。
            const auto& lineUpper = m_lines[lineIndex - 1];
            cursorPos = lineUpper.index + calcCursorPosInLine(xPos, m_font, Vec2::Zero(), lineUpper.text);
        }
    }

    // 下の行に移動
    if (KeyDown.down() || (KeyDown.pressedDuration() > SecondsF(0.33) && m_keyPressStopwatch > SecondsF(KEY_INTERVAL_V)))
    {
        m_keyPressStopwatch.restart();

        size_t lineIndex = getLineIndexOf(m_cursorPos, m_lines);
        if (lineIndex + 1 == m_lines.size()) {
            if (m_text.ends_with(U'\n')) {
                cursorPos = m_text.size();
            }
        }
        else if (lineIndex + 1 < m_lines.size()) {
            // 現在のカーソルのx座標を取得する。
            const auto& lineCurrent = m_lines[lineIndex];
            UITextIndexer indexer{ Vec2::Zero(), lineCurrent.text, m_font };
            indexer.nextUntil(m_cursorPos - lineCurrent.index);
            const double xPos = indexer.currentRegion().x;

            // 下の行でカーソル位置を取得する。
            const auto& lineUnder = m_lines[lineIndex + 1];
            cursorPos = lineUnder.index + calcCursorPosInLine(xPos, m_font, Vec2::Zero(), lineUnder.text);
        }
    }

    // 行頭に移動
    if (KeyHome.down()) {
        size_t lineIndex = getLineIndexOf(m_cursorPos, m_lines);
        if (lineIndex == m_lines.size()) {
            cursorPos = m_text.size();
        }
        else {
            const LineInfo& line = m_lines[lineIndex];
            cursorPos = line.index;
        }
    }

    // 行末に移動
    if (KeyEnd.down()) {
        size_t lineIndex = getLineIndexOf(m_cursorPos, m_lines);
        if (lineIndex == m_lines.size()) {
            cursorPos = m_text.size();
        }
        else {
            const LineInfo& line = m_lines[lineIndex];
            if (line.text.ends_with(U'\n')) {
                cursorPos = line.index + line.text.size() - 1;
            }
            else {
                cursorPos = line.index + line.text.size();
            }
        }
    }

    cursorPos = Clamp(cursorPos, (size_t)0, m_text.size());
    if (m_cursorPos != cursorPos) {
        setCursorPos(cursorPos, true);
    }
}

void SamlUI::TextBox::setCursorPos(size_t pos, bool moveView)
{
    m_cursorPos = Clamp(pos, size_t(0), m_text.size());

    if (moveView)
    {
        // pos番目の文字の描画領域を取得する。
        Vec2 textTL = TEXT_PADDING.xy();
        UITextIndexer indexer{ textTL, m_text, m_font };
        indexer.nextUntil(m_cursorPos);
        RectF regionToShow = indexer.currentRegion();

        // カーソルの左右の余白を見せたいので幅を広げる
        static const double rightPad = 10.0;
        static const double leftPad = 10.0;
        regionToShow.x -= rightPad;
        regionToShow.w += rightPad + leftPad;

        // regionToShowが表示領域に入るようにスクロールする。
        m_scrollView->moveToShow(regionToShow);
    }
}

// マウス関連の更新処理
void SamlUI::TextBox::updateMouse()
{
    Vec2 mousePos{ Cursor::Pos() };

    //--------------------------------------------------
    // クリック
    if (MouseL.pressed()) 
    {
        Vec2 textTL = m_scrollView->getRect().pos + m_scrollView->offset() + TEXT_PADDING.xy();
        size_t index = calcCursorPos(textTL, mousePos, m_font, m_lines);

        // テキストボックスでクリック、もしくはドラッグ開始した → 範囲選択開始
        if (isMouseOvered() && MouseL.down())
        {
            m_selectRange = IndexRange();
            m_selectRange->start = index;
            m_selectRange->current = index;
        }

        // テキストボックス上でドラッグを開始した場合、ドラッグしている限りは選択範囲を更新する。
        if (m_selectRange.has_value()) {
            // カーソル位置の更新
            m_cursorPos = index;

            m_selectRange->current = index;
        }
    }
}

void SamlUI::TextBox::setText(const String& text)
{
    m_text = text;
    m_lines.clear();

    LineInfo* currentLine = nullptr;

    for (UITextIndexer indexer{ Vec2::Zero(), m_text, m_font }; ; indexer.next())
    {
        if (indexer.isValid() == false) {
            if (currentLine != nullptr) {
                currentLine->text = m_text.substr(currentLine->index, indexer.currentIndex() - currentLine->index + 1);
            }
            return;
        }

        if (currentLine == nullptr)
        {
            m_lines.push_back(LineInfo());
            currentLine = &m_lines.back();
            currentLine->index = indexer.currentIndex();
            currentLine->offset = indexer.currentRegion().pos;
        }

        if (indexer.currentChar() == U'\n')
        {
            currentLine->text = m_text.substr(currentLine->index, indexer.currentIndex() - currentLine->index + 1);
            currentLine = nullptr;
        }
        else
        {
            currentLine->height = Max(currentLine->height, indexer.currentRegion().h);
        }
    }
}

void SamlUI::TextBox::insertText(const String& text, size_t index)
{

}