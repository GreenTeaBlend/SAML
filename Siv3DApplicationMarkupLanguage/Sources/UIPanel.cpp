#include "UIPanel.h"
#include "UIElement.h"

#include "Elements/Button.h"

using namespace s3d;
using namespace SamlUI;

UIPanel::UIPanel():
	m_elements(),
	m_mouseOveredElement(),
	m_focusingElement(),
	m_rect(0, 0, Window::ClientSize())
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
		panel->m_rootElement = panel->createElementRecursively(reader, nullptr);

		if (reader.nextSibling().isNull() == false) {
			// Root�v�f��2�ڈȍ~�͖�������B(WPF�Ɠ����d�l)
			// ���������Ă����牽���x�����o���悤�ɂ������B
		}

		return panel;
	}
	catch (std::exception ex)
	{
		// c++��O
		if (error != nullptr) { *error = Unicode::Widen(std::string(ex.what())); }
	}
	catch (const Error& ex)
	{
		// s3d��O
		if (error != nullptr) { *error = ex.what(); }
	}
	catch (...)
	{
		// ���̑��̗�O
		if (error != nullptr) { *error = U"Unexpected exception while parsing the xml."; }
	}

	return nullptr;
}

std::shared_ptr<UIElement> UIPanel::createElementRecursively(const XMLElement& xmlElement, const std::shared_ptr<UIElement>& parent)
{
	String className = xmlElement.name();

	std::shared_ptr<UIElement> uiElement = UIElement::create(className, *this, parent);
	m_elements.push_back(uiElement);

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

	// �q�v�f���ċA�I�ɐ���
	XMLElement childXml = xmlElement.firstChild();
	while (!childXml.isNull()) {
		auto childUI = createElementRecursively(childXml, uiElement);
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

	// �}�E�X�I�[�o�[����
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
		// �t�H�[�J�X����
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

void UIPanel::setRect(const RectF& rect)
{
	m_rect = rect;
}

void UIPanel::setDataContext(BindableObject& data)
{
	if (m_rootElement != nullptr) {
		m_rootElement->setDataContext(&data);
	}
}