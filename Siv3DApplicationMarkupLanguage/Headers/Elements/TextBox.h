#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"
#include "Components/ScrollView.h"

namespace s3d::SamlUI
{
    class TextBox : public UIElement
    {
    public:
        struct LineInfo {
            String text; // ���̍s�̕�����B���s�����Ȃ疖���ɉ��s�����B
            size_t index; // ���̍s�̐擪�̕����́A���̕�����ɂ�����v�f�ԍ��B
            Vec2 offset; // �擪�̕����̍���̍��W�BTextBox::getRect().pos����̑��΍��W�B
            double height; // �s�̍���
        };

        // �����̑I��͈�
        struct IndexRange {
            size_t start;
            size_t current;
            size_t length() const { return start > current ? start - current : current - start; }
        };

    private:

        Font m_font;

        String m_text;
        String m_textBindingProperty;

        size_t m_cursorPos;

        // m_text���s���Ƃɂ܂Ƃ߂����́B
        Array<LineInfo> m_lines;

        // �X�N���[���o�[�R���|�[�l���g
        std::unique_ptr<ScrollView> m_scrollView;

        // �����̑I��͈�
        Optional<IndexRange> m_selectRange;

        // �L�[�������ɂ��J�[�\���ړ��̂��߂̃^�C�}�[
        Stopwatch m_keyPressStopwatch;

        // �J�[�\���_�Ń^�C�}�[
        Stopwatch m_cursorStopwatch;
        
        bool m_isMouseOvered;
        bool m_isFocused;

        // �����ҏW�̍X�V����
        void updateText();
        // �}�E�X�֘A�̍X�V����
        void updateMouse();
        // �J�[�\���֘A�̍X�V����
        void updateCursor();

        // �X�N���[���o�[�������������̕����̕`��B�߂�l�͕\���̈�B
        SizeF drawInner(bool isMouseOvered);

        // �I��̈�̕`��
        void drawSelection();

    protected:

    public:

        TextBox();

        const String& getText() const { return m_text; }
        void setText(const String& text);

        ScrollBarVisibility isHorizontalScrollBarVisibility() const { return m_scrollView->getBarVisibility(ScrollBarDirection::Horizontal); }
        void setHorizontalScrollBarVisibility(ScrollBarVisibility visibility) { m_scrollView->setBarVisibility(ScrollBarDirection::Horizontal, visibility); }

        ScrollBarVisibility isVerticalScrollBarVisibility() const { return m_scrollView->getBarVisibility(ScrollBarDirection::Vertical); }
        void setVerticalScrollBarVisibility(ScrollBarVisibility visibility) { m_scrollView->setBarVisibility(ScrollBarDirection::Vertical, visibility); }

        // �J�[�\���̈ʒu���Z�b�g����B��������true�̏ꍇ�A�J�[�\�����\���ʒu�Ɏ��܂�悤�X�N���[�����s���܂��B
        void setCursorPos(size_t pos, bool moveView);

        /// <summary>
        /// �`�悷��B
        /// </summary>
        bool draw(bool mouseOverEnable = true) override;
    };
}