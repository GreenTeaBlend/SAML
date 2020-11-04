#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"
#include "Components/ScrollView.h"

namespace s3d::SamlUI
{
    class TextBox : public RectElement
    {
        Font m_font;

        String m_text;

        size_t m_cursorPos;

        // �X�N���[���o�[�R���|�[�l���g
        std::unique_ptr<ScrollView> m_scrollView;

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
        void setText(const String& text) { m_text = text; }

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