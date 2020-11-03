#include "Components/ScrollView.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    const double WHEEL_SPEED = 15.0;
}

ScrollView::ScrollView() :
    m_horizontalBarThickness(20.0),
    m_verticalBarThickness(20.0),
    m_horizontalBarState(),
    m_verticalBarState()
{

}

void ScrollView::draw(std::function<s3d::SizeF()> drawInner)
{
    const RectF& rect = getRect();

    // ステンシル矩形の設定
    RasterizerState rstatePre = Graphics2D::GetRasterizerState();
    RasterizerState rstateScissor = rstatePre;
    rstateScissor.scissorEnable = true;
    Graphics2D::Internal::SetRasterizerState(rstateScissor);

    // 内側を描画
    SizeF innerSize;
    {
        Vec2 offset{ m_horizontalBarState.pos * m_horizontalBarState.actualSize, m_verticalBarState.pos * m_verticalBarState.actualSize };
        Transformer2D transformer{ Mat3x2::Translate(-offset) };
        innerSize = drawInner();
    }

    // スクロールバーの描画判定と、テキスト描画領域の計算
    m_horizontalBarState.visible = true;
    m_verticalBarState.visible = true;
    RectF textRegion{ rect.pos, rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0) };

    // ステンシル矩形の設定を戻す
    Graphics2D::SetScissorRect(textRegion);
    Graphics2D::Internal::SetRasterizerState(rstatePre);

    // スクロールバーの状態の設定
    if (textRegion.w >= 0.0001 && textRegion.h >= 0.0001)
    {
        m_horizontalBarState.actualSize = innerSize.x;
        m_verticalBarState.actualSize = innerSize.y;

        m_horizontalBarState.length = innerSize.x > 1.0 ? Min(textRegion.w / innerSize.x, 1.0) : 1.0;
        m_verticalBarState.length = innerSize.y > 1.0 ? Min(textRegion.h / innerSize.y, 1.0) : 1.0;

        double wheel = WHEEL_SPEED * Mouse::Wheel();
        m_verticalBarState.pos += wheel / m_verticalBarState.actualSize;
        m_verticalBarState.pos = Clamp(m_verticalBarState.pos, 0.0, 1.0 - m_verticalBarState.length);
    }

    // スクロールバーの描画
    drawScrollBar();
}

void ScrollView::drawScrollBar()
{
    const RectF& rect = getRect();

    static const double PAD = 4.0;
    static const double ROUND = 2.0;

    //--------------------------------------------------
    // 垂直方向のバー
    Vec2 vbarPos = rect.tr() - Vec2{ m_verticalBarThickness, 0 };
    Vec2 vbarSize = Vec2(m_verticalBarThickness, rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0));
    RectF(vbarPos, vbarSize).draw(Palette::Wheat);

    // 内側のバー
    RectF(vbarPos + PAD * Vec2::One() + Vec2{ 0, m_verticalBarState.pos * (vbarSize.y - 2 * PAD) },
        vbarSize.x - 2 * PAD,
        m_verticalBarState.length * (vbarSize.y - 2 * PAD))
        .rounded(ROUND)
        .draw(Palette::Gray);

    //--------------------------------------------------
    // 水平方向のバー
    Vec2 hbarPos = rect.bl() - Vec2{ 0, m_horizontalBarThickness };
    Vec2 hbarSize = Vec2(rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), m_horizontalBarThickness);
    RectF(hbarPos, hbarSize).draw(Palette::Wheat);

    // 内側のバー
    RectF(hbarPos + PAD * Vec2::One() + Vec2{ m_horizontalBarState.pos * (hbarSize.x - 2 * PAD), 0 },
        m_horizontalBarState.length * (hbarSize.x - 2 * PAD),
        hbarSize.y - 2 * PAD)
        .rounded(ROUND)
        .draw(Palette::Gray);
}

void ScrollView::moveToShow(const RectF& region)
{
    // ゼロ除算の抑制
    if (m_horizontalBarState.actualSize < 0.0001 || m_verticalBarState.actualSize < 0.0001) {
        return;
    }

    // regionToShowの左側が見えるようになるためのバーの位置の最小値。以下、右上下についても同様。
    double leftRelativePos = region.x / m_horizontalBarState.actualSize;
    if (m_horizontalBarState.pos > leftRelativePos) { m_horizontalBarState.pos = leftRelativePos; }

    double rightRelativePos = (region.x + region.w) / m_horizontalBarState.actualSize - m_horizontalBarState.length;
    if (m_horizontalBarState.pos < rightRelativePos) { m_horizontalBarState.pos = rightRelativePos; }

    double topRelativePos = region.y / m_verticalBarState.actualSize;
    if (m_verticalBarState.pos > topRelativePos) { m_verticalBarState.pos = topRelativePos; }

    double bottomRelativePos = (region.y + region.h) / m_verticalBarState.actualSize - m_verticalBarState.length;
    if (m_verticalBarState.pos < bottomRelativePos) { m_verticalBarState.pos = bottomRelativePos; }
}