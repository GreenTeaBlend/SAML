#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

Button::Button() :
    m_text(U"button"),
    m_font(20)
{

}

void Button::enumratePropertyData(HashTable<String, PropertySetter>* datas)
{
    datas->insert(std::make_pair(U"Text",
        [&](UIElement* elm, const String& value) {
            ((Button*)elm)->m_text = value;
        }));

    RectElement::enumratePropertyData(datas);
}

bool Button::draw()
{
	//const int32 labelWidth = font(label).region().w;
	//const double width = _width.value_or(labelWidth + 40);

	const Rect rect{ getPosition().asPoint(), getSize().asPoint() };
	//const Vec2 labelPos(static_cast<int32>(rect.x + (width - labelWidth) / 2), center.y - font.height() / 2);

	const bool mouseOver = rect.rounded(4.8).mouseOver();
	const bool pushed = mouseOver && Cursor::OnClientRect() && MouseL.down();

	//if (enabled)
	//{
	rect.rounded(4.8)
		.draw(mouseOver ? ColorF(0.92, 0.96, 1.0) : ColorF(1.0))
		.drawFrame(1, 0, ColorF(0.67, pushed ? 0.0 : 1.0));

		m_font(m_text).drawAt(rect.center(), ColorF(0.2));
	//}
	//else
	//{
	//	rect.rounded(4.8)
	//		.draw(ColorF(0.92))
	//		.drawFrame(1, 0, ColorF(0.67));

	//	font(label).draw(labelPos, ColorF(0.67));
	//}

	if (mouseOver)
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

    //RectF rect{ getPosition(), getSize() };
    //rect.draw(Palette::White);
    //rect.drawFrame(1.0, Palette::Black);

    //m_font(m_text).drawAt(rect.center(), Palette::Black);

	return mouseOver;
}