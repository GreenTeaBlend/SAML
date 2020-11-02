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