#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"
#include "Elements/Panel.h"
#include "Components/ScrollView.h"

namespace s3d::SamlUI
{
    class ListView : public Panel
    {
    private:
        // スクロールバーコンポーネント
        ScrollView m_scrollView;

        // スクロールバーを除いた内側の部分の描画。戻り値は表示領域。
        SizeF drawInner(bool isMouseOvered);

    public:
        ListView();

        bool draw(bool mouseOverEnable = true) override;
    };
}