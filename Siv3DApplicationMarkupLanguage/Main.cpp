
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <fstream>

#include "SamlController.h"
#include "UIElement.h"
#include "Elements/TextBox.h"
using namespace s3d;

void Main()
{
	Window::Resize(1000, 600);

	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	// 大きさ 60 のフォントを用意
	const Font font(18);

	// 猫のテクスチャを用意
	const Texture cat(Emoji(U"🐈"));

	// 猫の座標
	Vec2 catPos(640, 450);

	TextBox textBox(font, Vec2(0, 0));

	SamlUI::UIElement::initialize();
	SamlUI::SamlController samlEditor{};
	SamlUI::SamlController samlPreview{};

	//String editorXml = U"<TextBox Name=\"textBox\" Size=\"(400, 600)\" Position=\"(0, 0)\"/>";
	String editorXml = U"<TextBox Name=\"textBox\" Size=\"(300, 400)\" Position=\"(50, 100)\"/>";

	samlEditor.parse(editorXml);

	uint64 lastIputTime = Time::GetMillisec();
	bool isTextEditted = false;

	String previewXml = String(U"<Button Position=\"(100, 200)\"/>");
	std::dynamic_pointer_cast<SamlUI::TextBox>(samlEditor.getElement(U"textBox"))->setText(previewXml);
	samlPreview.parse(previewXml);

	while (System::Update())
	{
		samlEditor.draw();

		if (samlPreview.isValid()) {
			Transformer2D transformer{ Mat3x2::Translate(Vec2(400, 0)), true };
			samlPreview.draw();
		}
		else {
			font(samlPreview.getError()).drawAt(Scene::Center(), Palette::Black);
		}

		String text{};
		if (TextInput::UpdateText(text, 0) != 0 || KeyBackspace.pressed())
		{
			lastIputTime = Time::GetMicrosec();
			isTextEditted = true;
		}

		if (isTextEditted && Time::GetMillisec() - lastIputTime > 500)
		{
			String xml = std::dynamic_pointer_cast<SamlUI::TextBox>(samlEditor.getElement(U"textBox"))->getText();
			samlPreview.parse(xml);

			isTextEditted = false;
		}
	}
}