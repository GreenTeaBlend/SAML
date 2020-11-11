#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d::SamlUI
{
    // ��`�̗̈������UI�v�f�̊��N���X
    class RectElement : public UIElement
    {
        // ���v�f�Ƃ̗]�� (�E�㍶��)
        Vec4 m_margin;

        // �v�f�̑傫�� (�e�v�f�ɍ��킹�ĕω�����)
        Vec2 m_size;

        // ����̍��W (�e�v�f�ɍ��킹�ĕω�����)
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