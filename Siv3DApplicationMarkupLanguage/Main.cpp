
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <fstream>

#include "SamlController.h"
#include "UIElement.h"
#include "Elements/TextBox.h"
using namespace s3d;

void Main()
{

	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	// 大きさ 60 のフォントを用意
	const Font font(18);

	// 猫のテクスチャを用意
	const Texture cat(Emoji(U"🐈"));

	// 猫の座標
	Vec2 catPos(640, 450);

	TextBox textBox(font, Vec2(0, 0));

	TextEditState text{};

	//String filePath = String(FileSystem::TemporaryDirectoryPath() + U"test.txt");
	//{
	//	std::ofstream ofs;
	//	ofs.open(filePath.narrow());
	//	ofs << xml.narrow();
	//	ofs.close();
	//}
	////Process::Spawn(U"C:\\WINDOWS\\system32\\notepad.exe", filePath);

	uint64 lastCheckTime = Time::GetMillisec();

	SamlUI::UIElement::initialize();
	SamlUI::SamlController samlEditor{};
	SamlUI::SamlController samlPreview{};

	String editorXml =
		String(U"<Button/>\n") +
		String(U"<TextBox Name=\"textBox\" Size=\"(200, 600)\" Position=\"(0, 0)\"/>");

	samlEditor.parse(editorXml);

	while (System::Update())
	{
		if (Time::GetMillisec() - lastCheckTime > 1000)
		{
			String xml = std::dynamic_pointer_cast<SamlUI::TextBox>(samlEditor.getElement(U"textBox"))->getText();
			samlPreview.parse(xml);

			lastCheckTime = Time::GetMillisec();
		}

		if (samlPreview.isValid()) {
			samlPreview.draw();
		}
		else {
			font(samlPreview.getError()).drawAt(Scene::Center(), Palette::Black);
		}
		
		samlEditor.draw();

		// マウスカーソルに追従する半透明の赤い円を描く
		Circle(Cursor::Pos(), 40).draw(ColorF(1, 0, 0, 0.5));
	}
}