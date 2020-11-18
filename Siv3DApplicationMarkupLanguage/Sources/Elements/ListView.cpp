#include "Elements/ListView.h"
#include "UITextIndexer.h"
#include "EventListener.h"

using namespace s3d;
using namespace SamlUI;

ListView::ListView() :
    m_scrollView()
{
    m_scrollView.setBarVisibility(ScrollBarDirection::Horizontal, ScrollBarVisibility::Hidden);
    m_scrollView.setBarVisibility(ScrollBarDirection::Vertical, ScrollBarVisibility::Visible);
}

bool ListView::draw(bool mouseOverEnable)
{
    bool isMouseOvered = mouseOverEnable && RectF{ getCurrentPosition(), getCurrentSize() }.mouseOver();

    // スクロールバーの矩形更新
    // TODO:毎フレームsetRectするのはちょっと歪なので、ScrollView::draw()に毎回矩形を渡すようにする
    m_scrollView.setRect(RectF{ getCurrentPosition(), getCurrentSize() });

    // スクロールバーと内側の描画
    m_scrollView.draw([&](bool isMouseOvered) {
        return drawInner(isMouseOvered);
        }, isMouseOvered);

    // 枠線の描画
    RectF rect{ getCurrentPosition(), getCurrentSize() };
    rect.drawFrame(1, 0, Palette::Gray);

    return false;
}

SizeF ListView::drawInner(bool isMouseOvered)
{
    double widthMax = 0.0;
    double heightMax = 0.0;
    for (auto* child : getChildren())
    {
        isMouseOvered = !child->draw(isMouseOvered) && isMouseOvered;
        Vec2 br = child->getCurrentPosition() + child->getCurrentSize() + child->getMargin().zw();
        widthMax = Max(widthMax, br.x);
        heightMax = Max(heightMax, br.y);
    }

    return SizeF{ widthMax, heightMax };
}