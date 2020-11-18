#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI {

    /// <summary>
    /// �q�v�f�������Ƃ��ł���UI�v�f
    /// </summary>
    class Panel : public UIElement
    {
        Array<UIElement*> m_children;

        RectF m_innerRect;

        void writeError(const String& message)
        {
#if _DEBUG
        Console.writeln(message);
#endif
        }

    protected:
        void setInnerRect(const RectF& rect);

        Panel();

    public:
        virtual ~Panel();

        void addChild(UIElement* child);

        void removeChild(UIElement* child);

        const Array<UIElement*> getChildren() const { return m_children; }

        /// <summary>
        /// ����UI�v�f�̎q�v�f���ʒu�E�T�C�Y�̎Q�l�ɂ����`�B
        /// </summary>
        const RectF& getInnerRect() { return m_innerRect; }

        UIElement* asUIElement() const { return (UIElement*)this; }
    };
}