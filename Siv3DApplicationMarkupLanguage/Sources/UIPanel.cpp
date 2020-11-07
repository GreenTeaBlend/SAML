#include "UIPanel.h"
#include "UIElement.h"

#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

UIPanel::UIPanel():
	m_elements(),
	m_mouseOveredElement(),
	m_focusingElement()
{
	if (!UIElement::hasInitialized) {
		UIElement::initialize();
	}
}

std::shared_ptr<UIPanel> UIPanel::create(String xml, String* error)
{
	auto panel = std::shared_ptr<UIPanel>(new UIPanel);

	XMLReader reader{};
	if (reader.open(s3d::Arg::code_<s3d::String>(xml)) == false) {
		if (error != nullptr) { *error = U"Invalid xml format."; }
		return nullptr;
	}

	try 
	{
		panel->m_rootElement = panel->createElement(reader);

		if (reader.nextSibling().isNull() == false) {
			// Root要素は2つ目以降は無視する。(WPFと同じ仕様)
			// 複数書いていたら何か警告を出すようにしたい。
		}

		return panel;
	}
	catch (std::exception ex)
	{
		// c++例外
		if (error != nullptr) { *error = Unicode::Widen(std::string(ex.what())); }
	}
	catch (const Error& ex)
	{
		// s3d例外
		if (error != nullptr) { *error = ex.what(); }
	}
	catch (...)
	{
		// その他の例外
		if (error != nullptr) { *error = U"Unexpected exception while parsing the xml."; }
	}

	return nullptr;
}

std::shared_ptr<UIElement> UIPanel::createElement(const XMLElement& xmlElement)
{
	String className = xmlElement.name();

	std::shared_ptr<UIElement> uiElement = UIElement::create(className, *this);
	m_elements.push_back(uiElement);

	for (auto& attrPair : xmlElement.attributes())
	{
		String propName = attrPair.first;
		String value = attrPair.second;
		uiElement->setProperty(propName, value);

		// いったん仮置き。UIElementのsetName()から設定できるようにしたい。
		if (propName == U"Name") {
			m_namedElements.insert(std::make_pair(value, uiElement));
		}
	}

	// 子要素を再帰的に生成
	XMLElement childXml = xmlElement.firstChild();
	while (!childXml.isNull()) {
		auto childUI = createElement(childXml);
		childXml = childXml.nextSibling();
	}

	return uiElement;
}


void UIPanel::drawUpdate()
{
	std::shared_ptr<UIElement> mouseOveredElement;
	for (auto& element : m_elements)
	{
		element->draw();
		/*if ()*/ {
			mouseOveredElement = element;
		}
	}

	// マウスオーバー処理
	if (m_mouseOveredElement != mouseOveredElement) 
	{
		auto& pre = m_mouseOveredElement;

		m_mouseOveredElement = mouseOveredElement;

		if (pre != nullptr) {
			pre->onMouseOverChanged();
		}
		if (m_mouseOveredElement != nullptr) {
			m_mouseOveredElement->onMouseOverChanged();
		}
	}

	if (MouseL.down()) 
	{
		// フォーカス処理
		if (m_focusingElement != m_mouseOveredElement)
		{
			auto& pre = m_focusingElement;

			m_focusingElement = m_mouseOveredElement;

			if (pre != nullptr) {
				pre->onFocusChanged();
			}
			if (m_focusingElement != nullptr) {
				m_focusingElement->onFocusChanged();
			}
		}
	}
}