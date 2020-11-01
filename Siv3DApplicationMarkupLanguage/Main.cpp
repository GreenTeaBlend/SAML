
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <fstream>

#include "SamlController.h"
using namespace s3d;

void Main()
{

	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	// 大きさ 60 のフォントを用意
	const Font font(30);

	// 猫のテクスチャを用意
	const Texture cat(Emoji(U"🐈"));

	// 猫の座標
	Vec2 catPos(640, 450);

	TextBox textBox(font, Vec2(0, 0));

	TextEditState text{};

	String xml =
		String(U"<Button/>\n") +
		String(U"<TextBox Position=\"(100, 400)\"/>");

	String filePath = String(FileSystem::TemporaryDirectoryPath() + U"test.txt");
	{
		std::ofstream ofs;
		ofs.open(filePath.narrow());
		ofs << xml.narrow();
		ofs.close();
	}
	Process::Spawn(U"C:\\WINDOWS\\system32\\notepad.exe", filePath);

	uint64 lastFileCheckTime = Time::GetMillisec();
	DateTime lastFileWriteTime = FileSystem::WriteTime(filePath).value();

	SamlUI::SamlController saml{};

	while (System::Update())
	{
		if (Time::GetMillisec() - lastFileCheckTime > 100)
		{
			lastFileCheckTime = Time::GetMillisec();

			DateTime fileWriteTime = FileSystem::WriteTime(filePath).value();
			if (fileWriteTime != lastFileWriteTime)
			{
				lastFileWriteTime = fileWriteTime;

				std::ifstream strm{ filePath.narrow() };
				std::string str((std::istreambuf_iterator<char>(strm)),
					std::istreambuf_iterator<char>());
				strm.close();

				saml.parse(Unicode::Widen(str));
			}
		}

		// テキストを画面の中心に描く
		//font(U"Hello, Siv3D!🐣").drawAt(Scene::Center(), Palette::Black);

		// 大きさをアニメーションさせて猫を表示する
		//cat.resized(100 + Periodic::Sine0_1(1s) * 20).drawAt(catPos);

		// マウスカーソルに追従する半透明の赤い円を描く
		Circle(Cursor::Pos(), 40).draw(ColorF(1, 0, 0, 0.5));

		//// [A] キーが押されたら
		//if (KeyA.down())
		//{
		//	// Hello とデバッグ表示する
		//	Print << U"Hello!";
		//}

		// ボタンが押されたら
		if (SimpleGUI::Button(U"Move the cat", Vec2(600, 20)))
		{
			// 猫の座標を画面内のランダムな位置に移動する
			catPos = RandomVec2(Scene::Rect());
		}

		if (saml.isValid()) {
			saml.draw();
		}
		else {
			font(saml.getError()).drawAt(Scene::Center(), Palette::Black);
		}

		//FileSystem::
		//FileSystem::WriteTime

		//if (SimpleGUI::TextBox(text, Vec2(0, 0)))
		//{
		//	XMLReader reader{};
		//	reader.open(s3d::Arg::code_<s3d::String>(text.text));
		//	font(reader.isOpen()).drawAt(Scene::Center(), Palette::Black);
		//	//font(reader.isNull()).drawAt(Scene::Center() + Vec2(0, 50), Palette::Black);
		//}
	}
}