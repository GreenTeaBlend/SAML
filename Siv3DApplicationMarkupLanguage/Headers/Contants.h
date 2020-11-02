#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    enum class ScrollBarVisibility {
        Auto, // 必要な状態では表示される
        Visible, // 常に表示
        Hidden // 常に非表示
        //,Disabled // 常に非表示にし、キー入力などによるスクロールそのものも無効化する。
    };

    [[nodiscard]] ScrollBarVisibility StringToScrollVarVisibility(const String& str);
    [[nodiscard]] String ScrollVarVisibilityToString(ScrollBarVisibility);
}