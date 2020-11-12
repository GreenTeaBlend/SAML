#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    class Button : public UIElement
    {
        String m_text;

        Font m_font;

    protected:

        friend class UIElement;
        friend class UIPanel;
        Button(UIPanel& panel);

    public:
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        const String& getText() const { return m_text; }
        void setText(const String& text) { m_text = text; }

        virtual void draw() override;
    };
}