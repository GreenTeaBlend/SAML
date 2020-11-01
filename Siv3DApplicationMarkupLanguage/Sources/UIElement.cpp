#include "UIElement.h"

using namespace Suml;

PropertyNotImplementedException::PropertyNotImplementedException(const char* elementName, const char* propertyName)
    : runtime_error("There is no property named \"" + std::string(propertyName) + "\" in \"" + std::string(elementName) + "\".")
{}

void UIElement::SetProperty(const String& name, const String& value) {
    throw PropertyNotImplementedException(typeid(*this).name(), name.narrow().c_str());
}