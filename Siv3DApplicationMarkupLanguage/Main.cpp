# include <Siv3D.hpp> // OpenSiv3D v0.4.3

#include "UIElement.h"
#include "Elements/TextBox.h"
#include "EventListener.h"
using namespace s3d;

void Main()
{
	Window::Resize(1000, 600);

	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	// 大きさ 60 のフォントを用意
	const Font font(18);

	auto textBox = SamlUI::TextBox();
	textBox.setAsRect(RectF(50, 100, 300, 400));

	while (System::Update())
	{
		// エディタの描画
		textBox.draw();
	}
}