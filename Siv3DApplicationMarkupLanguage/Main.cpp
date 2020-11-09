
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <fstream>

#include "UIPanel.h"
#include "UIElement.h"
#include "Elements/TextBox.h"
#include "EventListener.h"
using namespace s3d;

class Hoge {
public:
	void fuga(int value) {
		Console.writeln(U"fuga {}"_fmt(value));
	}
	void fuga2(int value, int value2) {
		Console.writeln(U"fuga2 {} {}"_fmt(value, value2));
	}
};

void Main()
{
	Hoge hoge{};

	{
		Event<int> event{};
		auto listener = MemberListener(&Hoge::fuga, &hoge);
		event += listener;
		event.invoke(123);
	}

	{
		Event<int, int> event{};
		auto listener = MemberListener(&Hoge::fuga2, &hoge);
		event += listener;
		event.invoke(123, 456);
	}

	//event.invoke(456);

	Window::Resize(1000, 600);

	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	// 大きさ 60 のフォントを用意
	const Font font(18);

	auto panel = SamlUI::UIPanel::create<SamlUI::TextBox>();
	auto* textBox = dynamic_cast<SamlUI::TextBox*>(panel->getRoot().get());
	textBox->setSize(Vec2{ 300, 400 });
	textBox->setMargin(Vec4{ 50, 100, 0, 0 });

	String initialXml =
		String(U"<Button Margin=\"(50, 50, 0, 0)\">\n") +
		U"\t<Button Margin=\"(0, 100, 0, 0)\"/>\n" +
		U"</Button>\n";
	textBox->setText(initialXml);

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