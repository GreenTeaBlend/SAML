#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d {
    template <class... _Args>
    class Event;
}

namespace s3d::SamlUI
{
    // 矩形の領域を持つUI要素の基底クラス
    class RectElement : public UIElement
    {
        // 他要素との余白 (右上左下)
        Vec4 m_margin;

        Vec2 m_size;

        // 左上の座標 (親要素に合わせて変化する)
        Vec2 m_pos;

        // 最後の座標関連の変数に変更が入ったか否か。
        bool m_isPosDirty;

        std::unique_ptr<Event<>> m_transformChangedEvent;
        void invokeTransformChangedEvent();

        // この要素とすべての子要素のm_isPosDirtyをtrueにセットする。
        void setPosDirtyRecursively();
    protected:

        friend class UIElement;
        friend class UIPanel;
        RectElement(UIPanel& panel);

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        virtual ~RectElement();

        const Vec4& getMargin() const { return m_margin; }
        void setMargin(const Vec4& margin) { 
            m_margin = margin;
            setPosDirtyRecursively();
            invokeTransformChangedEvent();
        }

        const Vec2& getSize() const { return m_size; }
        virtual void setSize(const Vec2& size) { 
            m_size = size;
            setPosDirtyRecursively();
            invokeTransformChangedEvent();
        }

        const Vec2& getPosition();

        Event<>& getTransformChangedEvent() const;
    };
}