#include "Elements/TextBox.h"
#include "UITextIndexer.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    // �E�㍶��
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
    // �L�[�{�[�h����e�L�X�g�����
    if (TextInput::GetRawInput().length() != 0) {
        String text = m_text;
        m_cursorPos = TextInput::UpdateText(text, m_cursorPos);
        setText(text);
    }

    // �J�[�\���ړ�
    int cursorPos = static_cast<int>(m_cursorPos);
    if (KeyRight.down()) { cursorPos++; }
    else if (KeyLeft.down()) { cursorPos--; }
    else if (KeyHome.down()) { cursorPos = 0; }
    else if (KeyEnd.down()) { cursorPos = (int)m_text.size(); }
    cursorPos = static_cast<size_t>(Clamp(cursorPos, 0, (int)m_text.size()));
    if (m_cursorPos != cursorPos) {
        setCursorPos(cursorPos, true);
    }

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

    // �����̍s���N���b�N�����Ƃ��āAx���W���Q�l�ɃN���b�N�����ꏊ��Ԃ��֐��B
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

    // �����񒆂̍Ō�̍s�����N���b�N�ʒu�������̏ꍇ�A�Ō�̍s�̒���x���W�݂̂��Q�l�ɃJ�[�\���ʒu�����߂�
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