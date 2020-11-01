#include "UIElement.h"
#include "Elements/Button.h"
#include "Elements/RectElement.h"

using namespace s3d;
using namespace SamlUI;

HashTable<String, UIElementData> UIElement::m_elementDatas{};

PropertyNotImplementedException::PropertyNotImplementedException(const char* elementName, const char* propertyName)
    : runtime_error("There is no property named \"" + std::string(propertyName) + "\" in \"" + std::string(elementName) + "\".")
{}

UIElementData::UIElementData(std::function<std::shared_ptr<class UIElement>()> factory) :
    m_instanceFactory(factory)
{

}

void UIElement::initialize()
{
    //m_elementDatas.insert(std::make_pair<String, UIElementData>(U"Button", ))
    HashTable<String, PropertySetter> datas;
    RectElement::enumratePropertyData(&datas);
}

void UIElement::enumratePropertyData(HashTable<String, PropertySetter>*)
{

}

void UIElement::SetProperty(const String& name, const String& value) {
    throw PropertyNotImplementedException(typeid(*this).name(), name.narrow().c_str());
}