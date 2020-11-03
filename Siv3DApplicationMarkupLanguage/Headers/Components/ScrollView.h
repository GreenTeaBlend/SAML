#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    enum class ScrollBarDirection {
        Horizontal,
        Vertical
    };

    // 各スクロールバーの状態
    struct ScrollBarState {
        bool visible;
        double length; // 全体に占める表示範囲の長さ。 0~1
        double pos; // バーの左(上)の位置。 0~1
        double actualSize; // 表示範囲外も含め、パネル全体の長さ (表示範囲=length*actualSize)
    };

    class ScrollView 
    {
        // スクロールバーの幅
        double m_horizontalBarThickness = 10.0;
        double m_verticalBarThickness = 10.0;

        ScrollBarVisibility m_isHorizontalScrollBarVisibility;
        ScrollBarVisibility m_isVerticalScrollBarVisibility;

        ScrollBarState m_horizontalBarState;
        ScrollBarState m_verticalBarState;

        // スクロールバーを含めた全体の領域
        s3d::RectF m_rect;

        // スクロールバー描画
        void drawScrollBar();

    public:
        ScrollView();

        /// <summary>
        /// スクロールバーと内側の領域を描画する。
        /// </summary>
        /// <param name="drawInner">内側を描画する関数。引数は内側をマウスオーバーしているか否かで、戻り値は内側領域のサイズ。</param>
        /// <param name="isMouseOvered">スクロールバーを含めたElement全体がマウスオーバーされているか。</param>
        void draw(std::function<s3d::SizeF(bool)> drawInner, bool isMouseOvered);

        const RectF& getRect() const { return m_rect; }
        void setRect(const RectF& rect) { m_rect = rect; }

        ScrollBarVisibility getBarVisibility(ScrollBarDirection dir) const
        {
            return dir == ScrollBarDirection::Horizontal ? m_isHorizontalScrollBarVisibility : m_isVerticalScrollBarVisibility;
        }
        void setBarVisibility(ScrollBarDirection dir, ScrollBarVisibility visibility)
        {
            if (dir == ScrollBarDirection::Horizontal) {
                m_isHorizontalScrollBarVisibility = visibility;
            }else {
                m_isVerticalScrollBarVisibility = visibility;
            }
        }

        /// <summary>
        /// 引数の領域が表示位置に収まるようにスクロールする
        /// </summary>
        /// <param name="rect">表示したい領域 (内側パネルにおける左上の点を(0,0)とした相対座標)</param>
        void moveToShow(const RectF& region);
    };
}