#include "SumlController.h"
#include "UIElement.h"

#include "Button.h"

using namespace Suml;

namespace {
	std::shared_ptr<UIElement> CreateElement(const String& name)
	{
		if (name == U"Button") {
			return std::shared_ptr<UIElement>((UIElement*)new Button());
		}

		throw std::exception(std::string("The element name \"" + name.narrow() + "\" was not implemented.").c_str());
	}

	std::shared_ptr<UIElement> CreateElement(const XMLElement& xmlElement)
	{
		std::shared_ptr<UIElement> uiElement = CreateElement(xmlElement.name());

		for (auto& attrPair : xmlElement.attributes())
		{
			uiElement->SetProperty(attrPair.first, attrPair.second);
		}

		return uiElement;
	}
}

SumlController::SumlController():
    m_isValid(false),
	m_error(U"XML has not been parsed."),
	m_elements()
{

}

void SumlController::parse(String xml)
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

bool SumlController::parseXmlElement(XMLElement* xmlElement)
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

void SumlController::draw()
{
	if (m_isValid == false) {
		return;
	}

	for (auto& element : m_elements)
	{
		element->draw();
	}
}