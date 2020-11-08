#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

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

        Array<const std::function<void()>*> m_transformChangedEvents;

        // この要素とすべての子要素のm_isPosDirtyをtrueにセットする。
        void setPosDirtyRecursively();
    protected:

        friend class UIElement;
        friend class UIPanel;
        RectElement(UIPanel& panel);

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        const Vec4& getMargin() const { return m_margin; }
        void setMargin(const Vec4& margin) { 
            m_margin = margin;
            setPosDirtyRecursively();
            for (auto& func : m_transformChangedEvents) { (*func)(); }
        }

        const Vec2& getSize() const { return m_size; }
        virtual void setSize(const Vec2& size) { 
            m_size = size;
            setPosDirtyRecursively();
            for (auto& func : m_transformChangedEvents) { (*func)(); }
        }

        const Vec2& getPosition();

        /// <summary>
        /// Transform要素 (Position, Size) に変更があった場合に呼ばれるハンドラを追加する。
        /// </summary>
        void hookTransformChangedEvent(const std::function<void()>* func) {
            m_transformChangedEvents.push_back(func); 
        }
        void unhookTransformChangedEvent(const std::function<void()>* func) {
            if (std::remove(m_transformChangedEvents.begin(), m_transformChangedEvents.end(), func) == m_transformChangedEvents.end()) {
                throw Error(U"Failed to remove the function. @RectElement.removeTransformChangedEvent");
            }
        }
    };
}