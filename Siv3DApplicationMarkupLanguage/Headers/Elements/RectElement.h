#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"

namespace s3d {
    template <class... _Args>
    class Event;
}

namespace s3d::SamlUI
{
    // ��`�̗̈������UI�v�f�̊��N���X
    class RectElement : public UIElement
    {
        // ���v�f�Ƃ̗]�� (�E�㍶��)
        Vec4 m_margin;

        Vec2 m_size;

        // ����̍��W (�e�v�f�ɍ��킹�ĕω�����)
        Vec2 m_pos;

        // �Ō�̍��W�֘A�̕ϐ��ɕύX�����������ۂ��B
        bool m_isPosDirty;

        std::unique_ptr<Event<>> m_transformChangedEvent;
        void invokeTransformChangedEvent();

        // ���̗v�f�Ƃ��ׂĂ̎q�v�f��m_isPosDirty��true�ɃZ�b�g����B
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