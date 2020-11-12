#include "Contants.h"

using namespace s3d;
using namespace SamlUI;

ScrollBarVisibility s3d::SamlUI::StringToScrollVarVisibility(const String& str)
{
    if (str == U"Auto") { return ScrollBarVisibility::Auto; }
    else if (str == U"Visible") { return ScrollBarVisibility::Visible; }
    else if (str == U"Hidden") { return ScrollBarVisibility::Hidden; }
    else throw ParseError(U"Failed to parse \"{}\" to ScrollBarVisibility"_fmt(str));
}

String s3d::SamlUI::ScrollVarVisibilityToString(ScrollBarVisibility value)
{
    switch (value) {
    case ScrollBarVisibility::Auto: return U"Auto";
    case ScrollBarVisibility::Visible: return U"Visible";
    case ScrollBarVisibility::Hidden: return U"Hidden";
    default:
        throw ParseError(U"Failed to convert ScrollBarVisibility \"{value}\" to String"_fmt((int)value));
    }
}

HorizontalAlignment s3d::SamlUI::StringToHorizontalAlignment(const String& str)
{
    if (str == U"Left") { return HorizontalAlignment::Left; }
    else if (str == U"Right") { return HorizontalAlignment::Right; }
    else if (str == U"Center") { return HorizontalAlignment::Center; }
    else if (str == U"Stretch") { return HorizontalAlignment::Stretch; }
    else throw ParseError(U"Failed to parse \"{}\" to HorizontalAlignment"_fmt(str));
}

String s3d::SamlUI::HorizontalAlignmentToString(HorizontalAlignment value)
{
    switch (value) {
    case HorizontalAlignment::Left: return U"Left";
    case HorizontalAlignment::Right: return U"Right";
    case HorizontalAlignment::Center: return U"Center";
    case HorizontalAlignment::Stretch: return U"Stretch";
    default:
        throw ParseError(U"Failed to convert HorizontalAlignment \"{value}\" to String"_fmt((int)value));
    }
}

VerticalAlignment s3d::SamlUI::StringToVerticalAlignment(const String& str)
{
    if (str == U"Top") { return VerticalAlignment::Top; }
    else if (str == U"Bottom") { return VerticalAlignment::Bottom; }
    else if (str == U"Center") { return VerticalAlignment::Center; }
    else if (str == U"Stretch") { return VerticalAlignment::Stretch; }
    else throw ParseError(U"Failed to parse \"{}\" to VerticalAlignment"_fmt(str));
}

String s3d::SamlUI::VerticalAlignmentToString(VerticalAlignment value)
{
    switch (value) {
    case VerticalAlignment::Top: return U"Top";
    case VerticalAlignment::Bottom: return U"Bottom";
    case VerticalAlignment::Center: return U"Center";
    case VerticalAlignment::Stretch: return U"Stretch";
    default:
        throw ParseError(U"Failed to convert VerticalAlignment \"{value}\" to String"_fmt((int)value));
    }
}