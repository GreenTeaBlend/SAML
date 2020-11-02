#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    // é`ÌÌæðÂUIvfÌîêNX
    class RectElement : public UIElement
    {
        Vec2 m_position;

        Vec2 m_size;

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        const Vec2& getPosition() const { return m_position; }
        void getPosition(const Vec2& position) { m_position = position; }

        const Vec2& getSize() const { return m_size; }
        void getSize(const Vec2& size) { m_size = size; }

        RectElement();
    };
}