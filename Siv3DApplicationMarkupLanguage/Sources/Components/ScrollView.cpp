#include "Components/ScrollView.h"
#include "EventListener.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    const double WHEEL_SPEED = 20.0;
}

ScrollView::ScrollView() :
    m_horizontalBarThickness(20.0),
    m_verticalBarThickness(20.0),
    m_horizontalBarState(),
    m_verticalBarState(),
    m_dragging()
{

}

void ScrollView::draw(std::function<s3d::SizeF(bool)> drawInner, bool)
{
    const RectF& rect = getRect();

    // 背景の描画
    rect.draw(Palette::White);

    // (描画前にスクロールバーの表示判定をしたい)
    m_horizontalBarState.visible = true;
    m_verticalBarState.visible = true;

    // スクロールバーを除いた、内側パネルの描画領域。
    RectF frameRect{ rect.pos, rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0) };
    bool mouseoverBar = frameRect.mouseOver();

    // ステンシル矩形の設定
    RasterizerState rstatePre = Graphics2D::GetRasterizerState();
    RasterizerState rstateScissor = rstatePre;
    rstateScissor.scissorEnable = true;
    Graphics2D::Internal::SetRasterizerState(rstateScissor);
    Graphics2D::SetScissorRect(frameRect);

    // 内側を描画
    SizeF innerSize;
    {
        Transformer2D transformer{ Mat3x2::Translate(getRect().pos + offset()), true };
        innerSize = drawInner(mouseoverBar);
    }

    // ステンシル矩形の設定を戻す
    Graphics2D::Internal::SetRasterizerState(rstatePre);

    // スクロールバーの状態の設定
    if (innerSize.x >= 0.0001 && innerSize.y >= 0.0001)
    {
        m_horizontalBarState.actualSize = innerSize.x;
        m_verticalBarState.actualSize = innerSize.y;

        m_horizontalBarState.length = innerSize.x > 1.0 ? Min(frameRect.w / innerSize.x, 1.0) : 1.0;
        m_verticalBarState.length = innerSize.y > 1.0 ? Min(frameRect.h / innerSize.y, 1.0) : 1.0;

        double wheel = WHEEL_SPEED * Mouse::Wheel();
        m_verticalBarState.pos += wheel / m_verticalBarState.actualSize;
        m_verticalBarState.pos = Clamp(m_verticalBarState.pos, 0.0, 1.0 - m_verticalBarState.length);
    }

    // スクロールバーの描画
    drawScrollBar();
}

void ScrollView::drawScrollBar()
{
    // ゼロ除算の抑制
    if (m_horizontalBarState.actualSize < 0.0001 || m_verticalBarState.actualSize < 0.0001) {
        return;
    }

    const RectF& rect = getRect();

    static const double PAD = 4.0;
    static const double ROUND = 2.0;

    //--------------------------------------------------
    // 垂直方向のバー
    Vec2 vbarPos = rect.tr() - Vec2{ m_verticalBarThickness, 0 };
    Vec2 vbarSize = Vec2(m_verticalBarThickness, rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0));
    RectF(vbarPos, vbarSize).draw(Palette::White);

    // 内側のバー
    RoundRect vbarRect = RectF(vbarPos + PAD * Vec2::One() + Vec2{ 0, m_verticalBarState.pos * (vbarSize.y - 2 * PAD) },
        vbarSize.x - 2 * PAD,
        m_verticalBarState.length * (vbarSize.y - 2 * PAD))
        .rounded(ROUND);
    if (vbarRect.mouseOver()) {
        vbarRect.draw(Palette::Lightgrey);
    }
    else {
        vbarRect.draw(Palette::Gray);
    }

    //--------------------------------------------------
    // 水平方向のバー
    Vec2 hbarPos = rect.bl() - Vec2{ 0, m_horizontalBarThickness };
    Vec2 hbarSize = Vec2(rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), m_horizontalBarThickness);
    RectF(hbarPos, hbarSize).draw(Palette::White);

    // 内側のバー
    RoundRect hbarRect = RectF(hbarPos + PAD * Vec2::One() + Vec2{ m_horizontalBarState.pos * (hbarSize.x - 2 * PAD), 0 },
        m_horizontalBarState.length * (hbarSize.x - 2 * PAD),
        hbarSize.y - 2 * PAD)
        .rounded(ROUND);
    if (hbarRect.mouseOver()) {
        hbarRect.draw(Palette::Lightgrey);
    }
    else {
        hbarRect.draw(Palette::Gray);
    }

    //--------------------------------------------------
    // ドラッグ処理
    if (m_dragging.has_value()) {
        if (MouseL.pressed()) {
            // ドラッグ中
            if (m_dragging == ScrollBarDirection::Vertical) {
                m_verticalBarState.pos = (Cursor::PosF().y - rect.y) / vbarSize.y - m_dragStartPos;
                m_verticalBarState.pos = Clamp(m_verticalBarState.pos, 0.0, 1.0 - m_verticalBarState.length);
            }
            else {
                m_horizontalBarState.pos = (Cursor::PosF().x - rect.x) / hbarSize.x - m_dragStartPos;
                m_horizontalBarState.pos = Clamp(m_horizontalBarState.pos, 0.0, 1.0 - m_horizontalBarState.length);
            }
        }
        else {
            m_dragging = Optional<ScrollBarDirection>();
        }
    }
    else if (vbarRect.leftPressed()){
        m_dragging = ScrollBarDirection::Vertical;
        m_dragStartPos = (Cursor::PosF().y - rect.y) / vbarSize.y - m_verticalBarState.pos;
    }
    else if (hbarRect.leftPressed()) {
        m_dragging = ScrollBarDirection::Horizontal;
        m_dragStartPos = (Cursor::PosF().x - rect.x) / hbarSize.x - m_horizontalBarState.pos;
    }
}

void ScrollView::moveToShow(const RectF& region)
{
    // ゼロ除算の抑制
    if (m_horizontalBarState.actualSize < 0.0001 || m_verticalBarState.actualSize < 0.0001) {
        return;
    }

    // regionToShowの左側が見えるように移動する。
    double leftRelativePos = region.x / m_horizontalBarState.actualSize;
    if (m_horizontalBarState.pos > leftRelativePos) { m_horizontalBarState.pos = leftRelativePos; }

    // 右側が見えるように移動
    double rightRelativePos = (region.x + region.w) / m_horizontalBarState.actualSize - m_horizontalBarState.length;
    if (m_horizontalBarState.pos < rightRelativePos) { m_horizontalBarState.pos = rightRelativePos; }

    // 上側が見えるように移動
    double topRelativePos = region.y / m_verticalBarState.actualSize;
    if (m_verticalBarState.pos > topRelativePos) { m_verticalBarState.pos = topRelativePos; }

    // 下側が見えるように移動
    double bottomRelativePos = (region.y + region.h) / m_verticalBarState.actualSize - m_verticalBarState.length;
    if (m_verticalBarState.pos < bottomRelativePos) { m_verticalBarState.pos = bottomRelativePos; }
}

Vec2 ScrollView::offset() const
{
    return -Vec2{ m_horizontalBarState.pos * m_horizontalBarState.actualSize, m_verticalBarState.pos * m_verticalBarState.actualSize };
}