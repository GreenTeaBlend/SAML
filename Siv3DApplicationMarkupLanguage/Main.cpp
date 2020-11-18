# include <Siv3D.hpp> // OpenSiv3D v0.4.3

#include "UIElement.h"
#include "Elements/TextBox.h"
#include "Elements/ListView.h"
#include "Elements/Button.h"
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
	textBox.setAsRect(RectF(50, 100, 100, 500));
	//auto button = SamlUI::Button();
	//button.setAsRect(RectF(2, 2, 300, 400));

	auto listView = SamlUI::ListView();
	listView.setAsRect(RectF(25, 100, 300, 400));
	listView.addChild(&textBox);

	while (System::Update())
	{
		// エディタの描画
		listView.draw();
		//textBox.draw();
	}
}