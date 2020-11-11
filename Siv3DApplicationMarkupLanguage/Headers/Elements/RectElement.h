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

        // 要素の大きさ (親要素に合わせて変化する)
        Vec2 m_size;

        // 左上の座標 (親要素に合わせて変化する)
        Vec2 m_pos;

        std::shared_ptr<RectElement> m_parent;

    protected:

        friend class UIElement;
        friend class UIPanel;
        RectElement(UIPanel& panel);

        virtual void updateTransform() override;

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        virtual ~RectElement();

        void setParent(const std::shared_ptr<UIElement>& parent) override;

        const Vec4& getMargin() const { return m_margin; }
        void setMargin(const Vec4& margin);

        const Vec2& getSize() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_size;
        }

        const Vec2& getPosition() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_pos;
        }
    };
}