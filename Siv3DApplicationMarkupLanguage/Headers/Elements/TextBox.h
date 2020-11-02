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

            // 全体の左上座標
            const Vec2 m_pos;
            // 文字の高さ
            const double m_height;

            Vec2 m_currentPos;
            // 現在の行の文字の高さ最大値 (複数の文字サイズを混合できるようにする)
            double m_currenMaxtHeight;

            size_t m_index;
        public:
            TextPoitionIndexer(Vec2 pos, const String& text, const Font& font);
            // 次の文字の位置を取得
            void next();
            bool isValid() const { return m_index < m_text.size(); }
            // 文字列内での現在の要素番号。
            const size_t& getIndex() const { return m_index; }
            // 現在の文字
            const String::value_type& getChar() const { return m_text.at(m_index); }
            // 現在の左上の座標
            const Vec2& getPos() const { return m_currentPos; }
            // 現在の文字のフォント (複数のフォントに対応予定)
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