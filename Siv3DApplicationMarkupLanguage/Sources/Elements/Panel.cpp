#include "Elements/Panel.h"

using namespace s3d;
using namespace SamlUI;

Panel::Panel() :
    m_children(),
    m_innerRect(0, 0, Window::ClientSize())
{
}

Panel::~Panel() {
    if (m_children.size() != 0) {
        auto children = Array<UIElement*>(m_children);
        for (auto* child : children) {
            removeChild(child);
        }
    }
}

void Panel::addChild(UIElement* child)
{
    if (child->m_parent != this) {
        child->m_parent = this;
        m_children.push_back(child);
        asUIElement()->setTransformDirty();
    }
    else if (child->m_parent != nullptr) {
        child->m_parent->removeChild(child);
        addChild(child);
        writeError(U"���ɑ���UIElement�̎q�v�f�ł��B @IParent.addChild()");
    }
    else {
        writeError(U"���ɐe�q�֌W�ł��B @IParent.addChild()");
    }
}

void Panel::removeChild(UIElement* child)
{
    if (child->m_parent == this) {
        child->m_parent = nullptr;
        int deleteCount = 0;
        for (auto it = m_children.begin(); it != m_children.end(); ) {
            if (*it == child) {
                it = m_children.erase(it);
                ++deleteCount;
            }
            else {
                ++it;
            }
        }

        if (deleteCount == 0) {
            writeError(U"������Child��������܂���ł����B @IParent.removeChild()");
        }
        else if (deleteCount >= 2) {
            writeError(U"2�ȏ��Children���q�b�g���܂����B @IParent.removeChild()");
        }

        asUIElement()->setTransformDirty();
        child->setTransformDirty();
    }
    else {
        writeError(U"�e�q�֌W�ł͂���܂���B @IParent.removeChild()");
    }
}

void Panel::setInnerRect(const RectF& rect) {
    m_innerRect = rect;
    asUIElement()->setTransformDirty();
}