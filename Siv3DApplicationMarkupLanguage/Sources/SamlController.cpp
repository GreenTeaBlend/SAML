#include "SamlController.h"
#include "UIElement.h"

#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

namespace 
{
	std::shared_ptr<UIElement> CreateElement(const XMLElement& xmlElement)
	{
		String className = xmlElement.name();

		std::shared_ptr<UIElement> uiElement = UIElement::create(className);

		for (auto& attrPair : xmlElement.attributes())
		{
			String propName = attrPair.first;
			String value = attrPair.second;
			uiElement->setProperty(propName, value);
		}

		return uiElement;
	}
}

SamlController::SamlController():
    m_isValid(false),
	m_error(U"XML has not been parsed."),
	m_elements(),
	m_mouseOveredElement(),
	m_focusingElement()
{

}

void SamlController::parse(String xml)
{
	m_isValid = false;
	m_error = U"";
	m_elements.clear();

	XMLReader reader{};
	if (reader.open(s3d::Arg::code_<s3d::String>(xml)) == false) {
		m_error = U"Invalid xml format.";
		return;
	}

	try 
	{
		// reader(root1番目)を読み込む。
		if (parseXmlElement(&reader) == false) {
			m_error = U"Invalid root element.";
			return;
		}

		// rootにある他の要素も読み込む。
		auto sibling = reader.nextSibling();
		while (true)
		{
			if (parseXmlElement(&sibling)) {
				sibling = sibling.nextSibling();
			}
			else {
				break;
			}
		}

		m_isValid = true;
	}
	catch (std::exception ex)
	{
		// c++例外
		m_error = Unicode::Widen(std::string(ex.what()));
		m_elements.clear();
	}
	catch (const Error& ex)
	{
		// s3d例外
		m_error = ex.what();
		m_elements.clear();
	}
	catch (...)
	{
		// その他の例外 (c?)
		m_error = U"Unexpected exception while parsing the xml.";
		m_elements.clear();
	}

}

bool SamlController::parseXmlElement(XMLElement* xmlElement)
{
	if (xmlElement->isNull()) 
	{
		return false;
	}

	m_elements.push_back(CreateElement(*xmlElement));

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

void SamlController::draw()
{
	if (m_isValid == false) {
		return;
	}

	std::shared_ptr<UIElement> mouseOveredElement;
	for (auto& element : m_elements)
	{
		if (element->draw()) {
			mouseOveredElement = element;
		}
	}

	// マウスオーバー処理
	if (m_mouseOveredElement != mouseOveredElement) 
	{
		if (m_mouseOveredElement != nullptr) {
			m_mouseOveredElement->onMouseOverEnd();
		}

		m_mouseOveredElement = mouseOveredElement;

		if (m_mouseOveredElement != nullptr) {
			m_mouseOveredElement->onMouseOverStart();
		}
	}

	if (MouseL.down()) 
	{
		// フォーカス処理
		if (m_focusingElement != m_mouseOveredElement)
		{
			if (m_focusingElement != nullptr) {
				m_focusingElement->onFocusEnd();
			}

			m_focusingElement = m_mouseOveredElement;

			if (m_focusingElement != nullptr) {
				m_focusingElement->onFocusStart();
			}
		}

		// クリック
		if (m_focusingElement != nullptr)
		{
			m_focusingElement->onClicked();
		}
	}
}