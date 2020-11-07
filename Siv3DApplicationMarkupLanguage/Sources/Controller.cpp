#include "Controller.h"
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

UIPanel* UIPanel::create(String xml)
{
	return nullptr;
	//auto* panel = new UIPanel();
	//m_elements.clear();

	//XMLReader reader{};
	//if (reader.open(s3d::Arg::code_<s3d::String>(xml)) == false) {
	//	m_error = U"Invalid xml format.";
	//	return;
	//}

	//try 
	//{
	//	// reader(root1番目)を読み込む。
	//	if (parseXmlElement(&reader) == false) {
	//		m_error = U"Invalid root element.";
	//		return;
	//	}

	//	// rootにある他の要素も読み込む。
	//	auto sibling = reader.nextSibling();
	//	while (true)
	//	{
	//		if (parseXmlElement(&sibling)) {
	//			sibling = sibling.nextSibling();
	//		}
	//		else {
	//			break;
	//		}
	//	}

	//	m_isValid = true;
	//}
	//catch (std::exception ex)
	//{
	//	// c++例外
	//	m_error = Unicode::Widen(std::string(ex.what()));
	//	m_elements.clear();
	//}
	//catch (const Error& ex)
	//{
	//	// s3d例外
	//	m_error = ex.what();
	//	m_elements.clear();
	//}
	//catch (...)
	//{
	//	// その他の例外 (c?)
	//	m_error = U"Unexpected exception while parsing the xml.";
	//	m_elements.clear();
	//}

}
/*
bool SamlController::parseXmlElement(XMLElement* xmlElement)
{
	if (xmlElement->isNull()) 
	{
		return false;
	}

	m_elements.push_back(createElement(*xmlElement));

	XMLElement child = xmlElement->firstChild();
	while (true) 
	{
		if (parseXmlElement(&child)) {
			child = child.nextSibling();
		}
		else {
			break;
		}
	}

	return true;
}

std::shared_ptr<UIElement> SamlController::createElement(const XMLElement& xmlElement)
{
	String className = xmlElement.name();

	std::shared_ptr<UIElement> uiElement = UIElement::create(className);

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

	return uiElement;
}
*/

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