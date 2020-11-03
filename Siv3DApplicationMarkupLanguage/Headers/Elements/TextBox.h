#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"
#include "Contants.h"

namespace s3d::SamlUI
{
    class TextBox : public RectElement
    {
        class TextPoitionIndexer 
        {
            const String& m_text;
            const Font& m_font;

            // �S�̂̍�����W
            const Vec2 m_tlPos;
            // �����̍���
            const double m_height;

            // ���݂̕����̕`��̈�
            RectF m_currentRegion;
            // ���݂̍s�̕����̍����ő�l (�����̕����T�C�Y�������ł���悤�ɂ���)
            double m_currenMaxHeight;

            size_t m_index;
        public:
            TextPoitionIndexer(Vec2 pos, const String& text, const Font& font);
            // ���̕����̈ʒu���擾
            void next();
            bool isValid() const { return m_index < m_text.size(); }
            // ��������ł̌��݂̗v�f�ԍ��B
            const size_t& getIndex() const { return m_index; }
            // ���݂̕���
            const String::value_type& getChar() const { return m_text.at(m_index); }
            // ���݂̕����̑傫��
            const RectF& getRegion() const { return m_currentRegion; }
            // ���݂̕����̃t�H���g (�����̃t�H���g�ɑΉ��\��)
            const Font& getFont() const { return m_font; }
            // ���݂̍s�̍ő卂�� == ���ɉ��s����Ƃ���y���W�ɉ��Z�����
            double getCurrenMaxHeight() const { return m_currenMaxHeight; }
        };

        struct ScrollBarState {
            bool visible;
            double length; // �S�̂ɐ�߂�\���͈͂̒����B 0~1
            double pos; // �o�[�̍�(��)�̈ʒu�B 0~1
            double actualSize; // �\���͈͊O���܂߁A�p�l���S�̂̒��� (�\���͈�=length*actualSize)
        };

        // �X�N���[���o�[�̕�
        double m_horizontalBarThickness = 10.0;
        double m_verticalBarThickness = 10.0;

        Font m_font;

        String m_text;

        bool m_isFocused;

        size_t m_cursorPos;

        ScrollBarVisibility m_isHorizontalScrollBarVisibility;
        ScrollBarVisibility m_isVerticalScrollBarVisibility;

        ScrollBarState m_horizontalBarState;
        ScrollBarState m_verticalBarState;

        // �X�N���[���o�[�`��
        void drawScrollBar();
    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        TextBox();

        const String& getText() const { return m_text; }
        void setText(const String& text) { m_text = text; }

        ScrollBarVisibility isHorizontalScrollBarVisibility() const { return m_isHorizontalScrollBarVisibility; }
        void setHorizontalScrollBarVisibility(ScrollBarVisibility enable) { m_isHorizontalScrollBarVisibility = enable; }

        ScrollBarVisibility isVerticalScrollBarVisibility() const { return m_isVerticalScrollBarVisibility; }
        void setVerticalScrollBarVisibility(ScrollBarVisibility enable) { m_isVerticalScrollBarVisibility = enable; }

        virtual bool draw() override;

        virtual void onClicked() override;

        virtual void onFocusStart() override;
        virtual void onFocusEnd() override;

        virtual void onMouseOvering() override;
    };
}