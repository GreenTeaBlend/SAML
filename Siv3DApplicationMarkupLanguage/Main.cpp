
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <fstream>

#include "UIPanel.h"
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

	auto panel = SamlUI::UIPanel::create<SamlUI::TextBox>();
	auto* textBox = dynamic_cast<SamlUI::TextBox*>(panel->getRoot().get());
	textBox->setSize(Vec2{ 300, 400 });
	textBox->setPosition(Vec2{ 50, 100 });
	textBox->setText(U"aiueo");

	uint64 lastIputTime = Time::GetMillisec();
	bool isTextEditted = false;

	std::shared_ptr<SamlUI::UIPanel> previewPanel;
	String previewError{};

	while (System::Update())
	{
		// エディタの描画
		panel->drawUpdate();

		if (TextInput::GetRawInput().length() != 0)
		{
			lastIputTime = Time::GetMicrosec();
			isTextEditted = true;
		}

		// 最後の文字入力から0.5秒以上経過したら、プレビューのPanelを再生成する。
		if (isTextEditted && Time::GetMillisec() - lastIputTime > 500)
		{
			String xml = textBox->getText();
			previewPanel = SamlUI::UIPanel::create(xml, &previewError);
			isTextEditted = false;
		}

		// プレビューの描画
		if (previewPanel != nullptr) {
			Transformer2D transformer{ Mat3x2::Translate(Vec2(400, 0)), true };
			previewPanel->drawUpdate();
		}
		else {
			font(previewError).drawAt(Scene::Center(), Palette::Black);
		}
	}
}