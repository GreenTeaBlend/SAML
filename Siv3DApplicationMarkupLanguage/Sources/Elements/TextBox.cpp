#include "Elements/TextBox.h"
#include "UITextIndexer.h"
#include "EventListener.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    // �E�㍶��
    const Vec4 TEXT_PADDING{ 6, 0, 30, 0 };

    // �L�[�������ɂ��J�[�\���ړ��̊Ԋu (������)
    const double KEY_INTERVAL_H{ 0.03 };
    // �L�[�������ɂ��J�[�\���ړ��̊Ԋu (�c����)
    const double KEY_INTERVAL_V{ 0.06 };

    /// <summary>
    /// �����̍s���N���b�N�����Ƃ��āAx���W���Q�l�ɃN���b�N�����ꏊ(�s���̕�����0�Ƃ����ԍ�)��Ԃ�
    /// </summary>
    /// <param name="mouseX">�}�E�X�ʒux���W</param>
    /// <param name="lineTL">mouseX�Ɠ������W�n�ł́A���ׂ����s�̍���̍��W</param>
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
    /// �����̍s���N���b�N�����Ƃ��āAx���W���Q�l�ɃN���b�N�����ꏊ(�s���̕�����0�Ƃ����ԍ�)��Ԃ�
    /// </summary>
    /// <param name="mouseX">�}�E�X�ʒux���W</param>
    /// <param name="lineTL">mouseX�Ɠ������W�n�ł́A���ׂ����s�̍���̍��W</param>
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

        // �����񒆂̍Ō�̍s�����N���b�N�ʒu�������̏ꍇ�A�Ō�̍s�̒���x���W�݂̂��Q�l�ɃJ�[�\���ʒu�����߂�
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
    /// ��������ł̔ԍ� index �ɂ��āA���̕�����̍s�̏W�� lines �̉��Ԗڂɑ����邩��Ԃ��B
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

        // �Ō�̍s�����s�����ŏI����Ă���Ȃ�A�͈͊O�̃J�[�\���͍Ō�̍s�̎��̍s�B
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
    // ��������
    updateText();

    // �L�[���͂ɂ��J�[�\���ړ�
    updateCursor();

    // �N���b�N�E�h���b�O�ȂǏ���
    updateMouse();

    // �X�N���[���o�[�̋�`�X�V
    // TODO:���t���[��setRect����̂͂�����Ƙc�Ȃ̂ŁAScrollView::draw()�ɖ����`��n���悤�ɂ���
    m_scrollView->setRect(RectF{ getPosition(), getSize() });

    // �X�N���[���o�[�Ɠ����̕`��
    m_scrollView->draw([&](bool isMouseOvered) {
        return drawInner(isMouseOvered);
        }, isMouseOvered());

    // �g���̕`��
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

    // 1�������`��
    for (UITextIndexer indexer{ textTL, m_text, m_font }; ; indexer.next())
    {
        // ���ꂩ��`�悷�镶���̗̈�
        const RectF& region = indexer.currentRegion();

        // �����̕`��
        // �J�[�\���̕`��̂��߂ɁA�����񂪏I��������1�������������[�v���񂷁B
        if (indexer.isValid()) {
            const String::value_type& c = indexer.currentChar();
            if (c != U'\n' && c != U'\r') {
                m_font(c).draw(region.pos, Palette::Black);
                widthMax = Max(widthMax, region.br().x);
                heightMax = Max(heightMax, region.br().y);
            }
            else if (c == U'\n') {
                // �Ō�ɉ��s�����ŏI�������Ƃ��̂��߂ɁA���s��̕����̉��[�̍��W��heightMax�ɐݒ�B
                double y = region.y + indexer.currentHeight() + m_font(U' ').region().h;
                heightMax = y;
            }
        }

        // �J�[�\���̕`��
        size_t index = indexer.currentIndex();
        if (index == m_cursorPos) {
            double h = m_font(U' ').region().h;
            Line(region.pos, region.pos + Vec2{ 0, h }).draw(Palette::Black);
        }

        if (!indexer.isValid()) {
            break;
        }
    }

    // �I��̈�̕`��
    drawSelection();

    // �������}�E�X�I�[�o�[����Ă���J�[�\���̌`��ς���B
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
    /// �����̍s�ɂ�����I���̋�`��`�悷��B
    /// </summary>
    /// <param name="start">�I��͈͊J�n�ԍ�(m_text���ł̔ԍ�)</param>
    /// <param name="end">�I��͈͏I���ԍ�(m_text���ł̔ԍ�)</param>
    static const auto drawLineSelection = [&](const LineInfo line)
    {
        String text = line.text/*.removed(U'\n')*/;

        // ���������s�����Ȃ�X�y�[�X�ɒu������B
        if (text.ends_with(U'\n')) {
            text = text.replaced(U'\n', U' ');
        }

        if (start >= line.index + text.size() || end <= line.index) {
            return;
        }

        // ��`�̍��[�̍��W
        double left = textTL.x + line.offset.x;
        if (start > line.index) {
            String textSubstr = text.substr(0, start - line.index);
            left += m_font(textSubstr).region().br().x;
        }

        // ��`�̉E�[�̍��W
        double right = textTL.x;
        if (end < line.index + text.size()) {
            String textSubstr = text.substr(0, end - line.index);
            right += m_font(textSubstr).region().br().x;
        }
        else {
            right += m_font(text).region().br().x;
        }

        // �`��
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
    // �L�[�{�[�h����e�L�X�g�����
    String inputText = TextInput::GetRawInput();

    if (inputText.length() != 0)
    {
        // �폜���������͂��ꂽ���ۂ�
        bool deleteCharInputed = false;
        for (char32_t c : inputText) {
            if (c == U'\b' || c == (char32_t)127) {
                deleteCharInputed = true;
                break;
            }
        }

        String text = m_text;
        if (deleteCharInputed && m_selectRange.has_value() && m_selectRange->length() != 0) {
            // �폜���������͂��ꂽ��I��͈͂��폜����B
            size_t min = Min(m_selectRange->start, m_selectRange->current);
            size_t max = Max(m_selectRange->start, m_selectRange->current);
            text = text.substr(0, min) + text.substr(max);
            m_cursorPos = min;
        }
        else {
            // �ʏ�̕����̑}��
            m_cursorPos = TextInput::UpdateText(text, m_cursorPos);
        }
        setText(text);

        // �͈͑I�������Ă����ꍇ�͉�������B
        if (m_selectRange.has_value()) {
            m_selectRange = Optional<IndexRange>();
        }
    }
}

void SamlUI::TextBox::updateCursor()
{
    size_t cursorPos = m_cursorPos;

    // ���E�ړ�
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

    // ��̍s�Ɉړ�
    if (KeyUp.down() || (KeyUp.pressedDuration() > SecondsF(0.33) && m_keyPressStopwatch > SecondsF(KEY_INTERVAL_V)))
    {
        m_keyPressStopwatch.restart();

        size_t lineIndex = getLineIndexOf(m_cursorPos, m_lines);
        if (lineIndex != 0) {
            // ���݂̃J�[�\����x���W���擾����B
            double xPos = 0;
            if (lineIndex != m_lines.size()) {
                const auto& lineCurrent = m_lines[lineIndex];
                UITextIndexer indexer{ Vec2::Zero(), lineCurrent.text, m_font };
                indexer.nextUntil(m_cursorPos - lineCurrent.index);
                xPos = indexer.currentRegion().x;
            }

            // ��̍s�ŃJ�[�\���ʒu���擾����B
            const auto& lineUpper = m_lines[lineIndex - 1];
            cursorPos = lineUpper.index + calcCursorPosInLine(xPos, m_font, Vec2::Zero(), lineUpper.text);
        }
    }

    // ���̍s�Ɉړ�
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
            // ���݂̃J�[�\����x���W���擾����B
            const auto& lineCurrent = m_lines[lineIndex];
            UITextIndexer indexer{ Vec2::Zero(), lineCurrent.text, m_font };
            indexer.nextUntil(m_cursorPos - lineCurrent.index);
            const double xPos = indexer.currentRegion().x;

            // ���̍s�ŃJ�[�\���ʒu���擾����B
            const auto& lineUnder = m_lines[lineIndex + 1];
            cursorPos = lineUnder.index + calcCursorPosInLine(xPos, m_font, Vec2::Zero(), lineUnder.text);
        }
    }

    // �s���Ɉړ�
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

    // �s���Ɉړ�
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
        // pos�Ԗڂ̕����̕`��̈���擾����B
        Vec2 textTL = TEXT_PADDING.xy();
        UITextIndexer indexer{ textTL, m_text, m_font };
        indexer.nextUntil(m_cursorPos);
        RectF regionToShow = indexer.currentRegion();

        // �J�[�\���̍��E�̗]�������������̂ŕ����L����
        static const double rightPad = 10.0;
        static const double leftPad = 10.0;
        regionToShow.x -= rightPad;
        regionToShow.w += rightPad + leftPad;

        // regionToShow���\���̈�ɓ���悤�ɃX�N���[������B
        m_scrollView->moveToShow(regionToShow);
    }
}

// �}�E�X�֘A�̍X�V����
void SamlUI::TextBox::updateMouse()
{
    Vec2 mousePos{ Cursor::Pos() };

    //--------------------------------------------------
    // �N���b�N
    if (MouseL.pressed()) 
    {
        Vec2 textTL = m_scrollView->getRect().pos + m_scrollView->offset() + TEXT_PADDING.xy();
        size_t index = calcCursorPos(textTL, mousePos, m_font, m_lines);

        // �e�L�X�g�{�b�N�X�ŃN���b�N�A�������̓h���b�O�J�n���� �� �͈͑I���J�n
        if (isMouseOvered() && MouseL.down())
        {
            m_selectRange = IndexRange();
            m_selectRange->start = index;
            m_selectRange->current = index;
        }

        // �e�L�X�g�{�b�N�X��Ńh���b�O���J�n�����ꍇ�A�h���b�O���Ă������͑I��͈͂��X�V����B
        if (m_selectRange.has_value()) {
            // �J�[�\���ʒu�̍X�V
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