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
            String text; // この行の文字列。改行されるなら末尾に改行文字。
            size_t index; // この行の先頭の文字の、元の文字列における要素番号。
            Vec2 offset; // 先頭の文字の左上の座標。TextBox::getRect().posからの相対座標。
            double height; // 行の高さ
        };

    private:

        Font m_font;

        String m_text;

        size_t m_cursorPos;

        // m_textを行ごとにまとめたもの。
        Array<LineInfo> m_lines;

        // スクロールバーコンポーネント
        std::unique_ptr<ScrollView> m_scrollView;
        
        void updateCursor();

        // スクロールバーを除いた内側の部分の描画。戻り値は表示領域。
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

        // カーソルの位置をセットする。第二引数がtrueの場合、カーソルが表示位置に収まるようスクロールが行われます。
        void setCursorPos(size_t pos, bool moveView);

        virtual bool draw() override;

        virtual void onClicked() override;
    };
}