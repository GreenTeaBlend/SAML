#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"
#include "Contants.h"

namespace s3d::SamlUI
{
    // ��`�̗̈������UI�v�f�̊��N���X
    class RectElement : public UIElement
    {
        // ���v�f�Ƃ̗]�� (�E�㍶��)
        Optional<Vec4> m_margin;

        // �T�C�Y�ݒ�l (m_margin���D��)
        Optional<Vec2> m_size;

        Optional<HorizontalAlignment> m_horizontalAlignment;
        Optional<VerticalAlignment> m_verticalAlignment;

        // �Ō�Ɍv�Z�����傫�� (�e�v�f�ɍ��킹�ĕω�����)
        Vec2 m_currentSize;

        // �Ō�Ɍv�Z�������W (�e�v�f�ɍ��킹�ĕω�����)
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