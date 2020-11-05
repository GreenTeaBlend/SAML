#include "Elements/TextBox.h"
#include "UITextIndexer.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    // �E�㍶��
    const Vec4 TEXT_PADDING{ 10, 0, 30, 0 };

    /// <summary>
    /// �����̍s���N���b�N�����Ƃ��āAx���W���Q�l�ɃN���b�N�����ꏊ(�s���̕�����0�Ƃ����ԍ�)��Ԃ�
    /// </summary>
    /// <param name="mouseX">�}�E�X�ʒux���W</param>
    /// <param name="lineTL">mouseX�Ɠ������W�n�ł́A���ׂ����s�̍���̍��W</param>
    size_t getMouseIndexInLine(double mouseX, const Font& font, const Vec2& lineTL, const String& lineText) 
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
    // �L�[�{�[�h����e�L�X�g�����
    if (TextInput::GetRawInput().length() != 0) {
        String text = m_text;
        m_cursorPos = TextInput::UpdateText(text, m_cursorPos);
        setText(text);
    }

    // �J�[�\���ړ�
    updateCursor();

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

    return rect.mouseOver();
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
                widthMax = Max(widthMax, (region.br().x - textTL.x));
                heightMax = Max(heightMax, (region.br().y - textTL.y));
            }
            else if (c == U'\n') {
                // �Ō�ɉ��s�����ŏI�������Ƃ��̂��߂ɁA���s��̕����̉��[�̍��W��heightMax�ɐݒ�B
                double y = region.y + indexer.currentHeight() + m_font(U' ').region().h;
                heightMax = (y - textTL.y);
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

    // �s�̃f�o�b�O�\��
    for (LineInfo& line : m_lines) {
        String text = line.text.removed(U'\n');
        m_font(text).region(textTL + line.offset).draw(ColorF(Palette::Green, 0.3));
    }

    // �������}�E�X�I�[�o�[����Ă���J�[�\���̌`��ς���B
    if (isMouseOvered){
        Cursor::RequestStyle(s3d::CursorStyle::IBeam);
    }

    return SizeF{ widthMax, heightMax } + TEXT_PADDING.zw();
}

void SamlUI::TextBox::updateCursor()
{
    size_t cursorPos = m_cursorPos;

    // ���E�ړ�
    if (KeyRight.down()) { cursorPos++; }
    if (KeyLeft.down()) { cursorPos = cursorPos >= 1 ? cursorPos - 1 : 0; }

    // ��̍s�Ɉړ�
    if (KeyUp.down()) {
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
            cursorPos = lineUpper.index + getMouseIndexInLine(xPos, m_font, Vec2::Zero(), lineUpper.text);
        }
    }

    // ���̍s�Ɉړ�
    if (KeyDown.down()) {
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
            cursorPos = lineUnder.index + getMouseIndexInLine(xPos, m_font, Vec2::Zero(), lineUnder.text);
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
        Vec2 textTL = TEXT_PADDING.xy();

        RectF regionToShow;
        for (UITextIndexer indexer{ textTL, m_text, m_font }; ; indexer.next())
        {
            if (indexer.currentIndex() == pos) {
                regionToShow = indexer.currentRegion();
                break;
            }

            // m_cursorPos�͕�����̊O�ɃZ�b�g����邱�Ƃ͂Ȃ��̂ŁA����return�͒ʂ�Ȃ��͂��B
            if (!indexer.isValid()) {
                return;
            }
        }

        // �e�L�X�g�̕`��J�n�_����̑��Έʒu
        regionToShow.moveBy(-textTL);

        //--------------------------------------------------
        // regionToShow���\���̈�ɓ���悤�ɃX�N���[������B
        m_scrollView->moveToShow(regionToShow);
    }
}

void SamlUI::TextBox::onClicked()
{
    Vec2 mousePos{ Cursor::Pos() };
    Vec2 textTL = m_scrollView->getRect().pos + m_scrollView->offset() + TEXT_PADDING.xy();

    for (LineInfo& line : m_lines) 
    {
        Vec2 lineTL = (textTL + line.offset);
        if (mousePos.y < lineTL.y + line.height) 
        {
            m_cursorPos = line.index + getMouseIndexInLine(mousePos.x, m_font, lineTL, line.text);
            return;
        }
    }

    // �����񒆂̍Ō�̍s�����N���b�N�ʒu�������̏ꍇ�A�Ō�̍s�̒���x���W�݂̂��Q�l�ɃJ�[�\���ʒu�����߂�
    if (m_text.size() == 0 || m_text.ends_with(U'\n')) {
        m_cursorPos = m_text.size();
    }
    else {
        auto& line = m_lines.back();
        Vec2 lineTL = (textTL + line.offset);
        m_cursorPos = line.index + getMouseIndexInLine(mousePos.x, m_font, lineTL, line.text);
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