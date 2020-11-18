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
            String text; // この行の文字列。改行されるなら末尾に改行文字。
            size_t index; // この行の先頭の文字の、元の文字列における要素番号。
            Vec2 offset; // 先頭の文字の左上の座標。TextBox::getRect().posからの相対座標。
            double height; // 行の高さ
        };

        // 文字の選択範囲
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

        // m_textを行ごとにまとめたもの。
        Array<LineInfo> m_lines;

        // スクロールバーコンポーネント
        std::unique_ptr<ScrollView> m_scrollView;

        // 文字の選択範囲
        Optional<IndexRange> m_selectRange;

        // キー長押しによるカーソル移動のためのタイマー
        Stopwatch m_keyPressStopwatch;

        // カーソル点滅タイマー
        Stopwatch m_cursorStopwatch;
        
        bool m_isMouseOvered;
        bool m_isFocused;

        // 文字編集の更新処理
        void updateText();
        // マウス関連の更新処理
        void updateMouse();
        // カーソル関連の更新処理
        void updateCursor();

        // スクロールバーを除いた内側の部分の描画。戻り値は表示領域。
        SizeF drawInner(bool isMouseOvered);

        // 選択領域の描画
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

        // カーソルの位置をセットする。第二引数がtrueの場合、カーソルが表示位置に収まるようスクロールが行われます。
        void setCursorPos(size_t pos, bool moveView);

        /// <summary>
        /// 描画する。
        /// </summary>
        bool draw(bool mouseOverEnable = true) override;
    };
}