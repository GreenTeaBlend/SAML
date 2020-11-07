#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    // ��`�̗̈������UI�v�f�̊��N���X
    class RectElement : public UIElement
    {
        RectF m_rect;
    protected:

        friend class UIElement;
        friend class UIPanel;
        RectElement(UIPanel& panel);

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        const RectF& getRect() const { return m_rect; }
        virtual void setRect(const RectF& rect) { m_rect = rect; }

        const Vec2& getPosition() const { return m_rect.pos; }
        virtual void setPosition(const Vec2& pos) { m_rect.pos = pos; }

        const Vec2& getSize() const { return m_rect.size; }
        virtual void setSize(const Vec2& size) { m_rect.size = size; }
    };
}