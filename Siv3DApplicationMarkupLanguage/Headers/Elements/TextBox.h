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

            // 全体の左上座標
            const Vec2 m_tlPos;
            // 文字の高さ
            const double m_height;

            // 現在の文字の描画領域
            RectF m_currentRegion;
            // 現在の行の文字の高さ最大値 (複数の文字サイズを混合できるようにする)
            double m_currenMaxHeight;

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
            // 現在の文字の大きさ
            const RectF& getRegion() const { return m_currentRegion; }
            // 現在の文字のフォント (複数のフォントに対応予定)
            const Font& getFont() const { return m_font; }
            // 現在の行の最大高さ == 次に改行するときのy座標に加算する量
            double getCurrenMaxHeight() const { return m_currenMaxHeight; }
        };

        struct ScrollBarState {
            bool visible;
            double length; // 全体に占める表示範囲の長さ。 0~1
            double pos; // バーの左(上)の位置。 0~1
            double actualSize; // 表示範囲外も含め、パネル全体の長さ (表示範囲=length*actualSize)
        };

        // スクロールバーの幅
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

        // スクロールバー描画
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