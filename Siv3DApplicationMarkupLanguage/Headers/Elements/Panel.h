#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI {

    /// <summary>
    /// 子要素を持つことができるUI要素
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
        /// このUI要素の子要素が位置・サイズの参考にする矩形。
        /// </summary>
        const RectF& getInnerRect() { return m_innerRect; }

        UIElement* asUIElement() const { return (UIElement*)this; }
    };
}