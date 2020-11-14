
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <fstream>

#include "UIPanel.h"
#include "UIElement.h"
#include "Elements/TextBox.h"
#include "EventListener.h"
#include "BindableObject.h"
using namespace s3d;

class Hoge : public s3d::SamlUI::BindableObject
{
public:
	String m_text{};
	void setText(const String& text) {
		m_text = text;
		raisePropertyChanged(U"HogeText");
	}

	Hoge() {
		registerProperty<String>(U"HogeText",
			[&](String& value) {
				value = this->m_text;
			},
			[&](const String& value) { this->m_text = value; }
		);
	}
};

void Main()
{
	Hoge hoge;

	Window::Resize(1000, 600);

	// 背景を水色にする
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	// 大きさ 60 のフォントを用意
	const Font font(18);

	auto panel = SamlUI::UIPanel::create<SamlUI::TextBox>();
	panel->setRect(RectF(0, 0, 400, 600));
	auto* textBox = dynamic_cast<SamlUI::TextBox*>(panel->getRoot().get());
	//textBox->setSize(Vec2{ 300, 400 });
	//textBox->setMargin(Vec4{ 50, 100, 20, 20 });

	String initialXml =
		String(U"<Button Margin=\"(50, 50, 0, 0)\">\n") +
		U"\t<TextBox Margin=\"(0, 100, 0, 0)\"/>\n" +
		U"</Button>\n";
	textBox->setText(initialXml);

	uint64 lastIputTime = Time::GetMillisec();
	bool isTextEditted = false;

	std::shared_ptr<SamlUI::UIPanel> previewPanel;
	String previewError{};

	while (System::Update())
	{
		if (MouseR.down()) {
			hoge.setText(U"{}"_fmt(Time::GetMillisec()));
		}

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
			if (previewPanel != nullptr) {
				previewPanel->setRect(RectF(400, 0, 600, 600));
				previewPanel->setDataContext(hoge);
			}
			isTextEditted = false;
		}

		// プレビューの描画
		if (previewPanel != nullptr) {
			previewPanel->drawUpdate();
		}
		else {
			font(previewError).drawAt(Scene::Center(), Palette::Black);
		}
	}
}