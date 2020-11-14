#include "BindableObject.h"
#include "UIElement.h"

using namespace s3d;
using namespace SamlUI;

BindableObject::BindableObject()
{

}

void BindableObject::raisePropertyChanged(const String& name)
{
    for (auto* element : m_elements) {
        element->onPropertyChangedRecursively(name);
    }
}

BindableObject::~BindableObject()
{

}