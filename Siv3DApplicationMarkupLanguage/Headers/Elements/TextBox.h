#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"

namespace s3d::SamlUI
{
    class TextBox : public RectElement
    {
        class TextPoitionIndexer 
        {
            const String& m_text;
            const Font& m_font;

            // �S�̂̍�����W
            const Vec2 m_pos;
            // �����̍���
            const double m_height;

            Vec2 m_currentPos;
            // ���݂̍s�̕����̍����ő�l (�����̕����T�C�Y�������ł���悤�ɂ���)
            double m_currenMaxtHeight;

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
            // ���݂̍���̍��W
            const Vec2& getPos() const { return m_currentPos; }
            // ���݂̕����̃t�H���g (�����̃t�H���g�ɑΉ��\��)
            const Font& getFont() const { return m_font; }
        };

        Font m_font;

        String m_text;

        bool m_isFocused;

        int m_cursorPos;
    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        TextBox();

        const String& getText() const { return m_text; }
        void setText(const String& text) { m_text = text; }

        virtual bool draw() override;

        virtual void onClicked() override;

        virtual void onFocusStart() override;
        virtual void onFocusEnd() override;

        virtual void onMouseOvering() override;
    };
}