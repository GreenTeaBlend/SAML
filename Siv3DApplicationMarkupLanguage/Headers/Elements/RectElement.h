#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"
#include "Contants.h"

namespace s3d::SamlUI
{
    // 矩形の領域を持つUI要素の基底クラス
    class RectElement : public UIElement
    {
        // 他要素との余白 (右上左下)
        Optional<Vec4> m_margin;

        // サイズ設定値 (m_marginより優先)
        Optional<Vec2> m_size;

        Optional<HorizontalAlignment> m_horizontalAlignment;
        Optional<VerticalAlignment> m_verticalAlignment;

        // 最後に計算した大きさ (親要素に合わせて変化する)
        Vec2 m_currentSize;

        // 最後に計算した座標 (親要素に合わせて変化する)
        Vec2 m_currentPos;

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

        const Optional<Vec4>& getMargin() const { return m_margin; }
        void setMargin(const Optional<Vec4>& margin);

        const Optional<Vec2>& getSize() const { return m_size; }
        void setSize(const Optional<Vec2>& size);

        const Optional<HorizontalAlignment>& getHorizontalAlignment() const { return m_horizontalAlignment; }
        void setHorizontalAlignment(const Optional<HorizontalAlignment>& alignment);

        const Optional<VerticalAlignment>& getVerticalAlignment() const { return m_verticalAlignment; }
        void setVerticalAlignment(const Optional<VerticalAlignment>& alignment);

        const Vec2& getSize() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentSize;
        }

        const Vec2& getPosition() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentPos;
        }
    };
}