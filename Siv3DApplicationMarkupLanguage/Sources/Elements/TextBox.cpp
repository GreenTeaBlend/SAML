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
    m_cursorPos = TextInput::UpdateText(m_text, m_cursorPos);

    int cursorPos = static_cast<int>(m_cursorPos);
    if (KeyRight.down()) { cursorPos++; }
    else if (KeyLeft.down()) { cursorPos--; }
    m_cursorPos = static_cast<size_t>(Clamp(cursorPos, 0, (int)m_text.size()));

    // �`��
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

    if (KeyHome.down()) setCursorPos(0, true);
    if (KeyEnd.down()) setCursorPos(m_text.size(), true);

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
    Vec2 charPos{ getPosition() };

    // �N���b�N�����ӏ������������������
    if (mousePos.x < charPos.x || mousePos.y < charPos.y) {
        return;
    }

    Vec2 textTL = m_scrollView->getRect().pos + m_scrollView->offset() + TEXT_PADDING.xy();
    for (UITextIndexer indexer{ textTL, m_text, m_font }; indexer.isValid(); indexer.next())
    {
        RectF charRect = indexer.currentRegion();

        if (mousePos.x <= charRect.br().x && mousePos.y <= charRect.br().y) {
            if (mousePos.x <= charRect.center().x) {
                m_cursorPos = indexer.currentIndex();
            }
            else {
                m_cursorPos = indexer.currentIndex() + 1;
            }
            return;
        }
    }
}