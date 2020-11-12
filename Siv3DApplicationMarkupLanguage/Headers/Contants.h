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
    [[nodiscard]] String ScrollVarVisibilityToString(ScrollBarVisibility value);

    enum class HorizontalAlignment {
        Left, // 左に寄せる
        Right, // 右に寄せる
        Center, // 中央に寄せる
        Stretch // 左右両方に広げる
    };

    [[nodiscard]] HorizontalAlignment StringToHorizontalAlignment(const String& str);
    [[nodiscard]] String HorizontalAlignmentToString(HorizontalAlignment value);

    enum class VerticalAlignment {
        Top, // 上に寄せる
        Bottom, // 下に寄せる
        Center, // 中央に寄せる
        Stretch // 上下両方に広げる
    };

    [[nodiscard]] VerticalAlignment StringToVerticalAlignment(const String& str);
    [[nodiscard]] String VerticalAlignmentToString(VerticalAlignment value);
}