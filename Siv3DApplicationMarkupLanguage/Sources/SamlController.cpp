#include "SamlController.h"
#include "UIElement.h"

#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

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
		// reader(root1�Ԗ�)��ǂݍ��ށB
		if (parseXmlElement(&reader) == false) {
			m_error = U"Invalid root element.";
			return;
		}

		// root�ɂ��鑼�̗v�f���ǂݍ��ށB
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
		// c++��O
		m_error = Unicode::Widen(std::string(ex.what()));
		m_elements.clear();
	}
	catch (const Error& ex)
	{
		// s3d��O
		m_error = ex.what();
		m_elements.clear();
	}
	catch (...)
	{
		// ���̑��̗�O (c?)
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

		// �������񉼒u���BUIElement��setName()����ݒ�ł���悤�ɂ������B
		if (propName == U"Name") {
			m_namedElements.insert(std::make_pair(value, uiElement));
		}
	}

	return uiElement;
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

	// �}�E�X�I�[�o�[����
	if (m_mouseOveredElement != mouseOveredElement) 
	{
		if (m_mouseOveredElement != nullptr) {
			m_mouseOveredElement->setMouseOvered(false);
		}

		m_mouseOveredElement = mouseOveredElement;

		if (m_mouseOveredElement != nullptr) {
			m_mouseOveredElement->setMouseOvered(true);
		}
	}

	if (MouseL.down()) 
	{
		// �t�H�[�J�X����
		if (m_focusingElement != m_mouseOveredElement)
		{
			if (m_focusingElement != nullptr) {
				m_focusingElement->setFocusing(false);
			}

			m_focusingElement = m_mouseOveredElement;

			if (m_focusingElement != nullptr) {
				m_focusingElement->setFocusing(true);
			}
		}

		// �N���b�N
		if (m_focusingElement != nullptr)
		{
			m_focusingElement->onClicked();
		}
	}
}