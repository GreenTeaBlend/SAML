#pragma once
#include <Siv3D.hpp>
#include "RectElement.h"
#include "Components/ScrollView.h"

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

        Font m_font;

        String m_text;

        size_t m_cursorPos;

        // スクロールバーコンポーネント
        std::unique_ptr<ScrollView> m_scrollView;

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
        void setText(const String& text) { m_text = text; }

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