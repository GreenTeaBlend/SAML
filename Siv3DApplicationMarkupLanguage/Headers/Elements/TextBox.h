#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"
#include "Components/ScrollView.h"

namespace s3d::SamlUI
{
    class TextBox : public RectElement
    {
    public:
        struct LineInfo {
            String text; // ���̍s�̕�����B���s�����Ȃ疖���ɉ��s�����B
            size_t index; // ���̍s�̐擪�̕����́A���̕�����ɂ�����v�f�ԍ��B
            Vec2 offset; // �擪�̕����̍���̍��W�BTextBox::getRect().pos����̑��΍��W�B
            double height; // �s�̍���
        };

    private:

        Font m_font;

        String m_text;

        size_t m_cursorPos;

        // m_text���s���Ƃɂ܂Ƃ߂����́B
        Array<LineInfo> m_lines;

        // �X�N���[���o�[�R���|�[�l���g
        std::unique_ptr<ScrollView> m_scrollView;
        
        void updateCursor();

        // �X�N���[���o�[�������������̕����̕`��B�߂�l�͕\���̈�B
        SizeF drawInner(bool isMouseOvered);
    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        TextBox();

        virtual void setPosition(const Vec2& pos) override {
            RectElement::setPosition(pos); 
            m_scrollView->setRect(RectF{ pos, m_scrollView->getRect().size }); 
        }
        virtual void setSize(const Vec2& size) {
            RectElement::setSize(size);
            m_scrollView->setRect(RectF{ m_scrollView->getRect().pos, size });
        }

        const String& getText() const { return m_text; }
        void setText(const String& text);
        void insertText(const String& text, size_t index);

        ScrollBarVisibility isHorizontalScrollBarVisibility() const { return m_scrollView->getBarVisibility(ScrollBarDirection::Horizontal); }
        void setHorizontalScrollBarVisibility(ScrollBarVisibility visibility) { m_scrollView->setBarVisibility(ScrollBarDirection::Horizontal, visibility); }

        ScrollBarVisibility isVerticalScrollBarVisibility() const { return m_scrollView->getBarVisibility(ScrollBarDirection::Vertical); }
        void setVerticalScrollBarVisibility(ScrollBarVisibility visibility) { m_scrollView->setBarVisibility(ScrollBarDirection::Vertical, visibility); }

        // �J�[�\���̈ʒu���Z�b�g����B��������true�̏ꍇ�A�J�[�\�����\���ʒu�Ɏ��܂�悤�X�N���[�����s���܂��B
        void setCursorPos(size_t pos, bool moveView);

        virtual bool draw() override;

        virtual void onClicked() override;
    };
}