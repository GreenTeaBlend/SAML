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

        Vec2 m_size;

        // ����̍��W (�e�v�f�ɍ��킹�ĕω�����)
        Vec2 m_pos;

        // �Ō�̍��W�֘A�̕ϐ��ɕύX�����������ۂ��B
        bool m_isPosDirty;

        Array<const std::function<void()>*> m_transformChangedEvents;

        // ���̗v�f�Ƃ��ׂĂ̎q�v�f��m_isPosDirty��true�ɃZ�b�g����B
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
        /// Transform�v�f (Position, Size) �ɕύX���������ꍇ�ɌĂ΂��n���h����ǉ�����B
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