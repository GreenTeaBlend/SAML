#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    enum class ScrollBarDirection {
        Horizontal,
        Vertical
    };

    // �e�X�N���[���o�[�̏��
    struct ScrollBarState {
        bool visible;
        double length; // �S�̂ɐ�߂�\���͈͂̒����B 0~1
        double pos; // �o�[�̍�(��)�̈ʒu�B 0~1
        double actualSize; // �\���͈͊O���܂߁A�p�l���S�̂̒��� (�\���͈�=length*actualSize)
    };

    class ScrollView 
    {
        // �X�N���[���o�[�̕�
        double m_horizontalBarThickness = 10.0;
        double m_verticalBarThickness = 10.0;

        ScrollBarVisibility m_isHorizontalScrollBarVisibility;
        ScrollBarVisibility m_isVerticalScrollBarVisibility;

        ScrollBarState m_horizontalBarState;
        ScrollBarState m_verticalBarState;

        // �X�N���[���o�[���܂߂��S�̗̂̈�
        s3d::RectF m_rect;

        // �X�N���[���o�[�`��
        void drawScrollBar();

    public:
        ScrollView();

        /// <summary>
        /// �X�N���[���o�[�Ɠ����̗̈��`�悷��B
        /// </summary>
        /// <param name="drawInner">������`�悷��֐��B�����͓������}�E�X�I�[�o�[���Ă��邩�ۂ��ŁA�߂�l�͓����̈�̃T�C�Y�B</param>
        /// <param name="isMouseOvered">�X�N���[���o�[���܂߂�Element�S�̂��}�E�X�I�[�o�[����Ă��邩�B</param>
        void draw(std::function<s3d::SizeF(bool)> drawInner, bool isMouseOvered);

        const RectF& getRect() const { return m_rect; }
        void setRect(const RectF& rect) { m_rect = rect; }

        ScrollBarVisibility getBarVisibility(ScrollBarDirection dir) const
        {
            return dir == ScrollBarDirection::Horizontal ? m_isHorizontalScrollBarVisibility : m_isVerticalScrollBarVisibility;
        }
        void setBarVisibility(ScrollBarDirection dir, ScrollBarVisibility visibility)
        {
            if (dir == ScrollBarDirection::Horizontal) {
                m_isHorizontalScrollBarVisibility = visibility;
            }else {
                m_isVerticalScrollBarVisibility = visibility;
            }
        }

        /// <summary>
        /// �����̗̈悪�\���ʒu�Ɏ��܂�悤�ɃX�N���[������
        /// </summary>
        /// <param name="rect">�\���������̈� (�����p�l���ɂ����鍶��̓_��(0,0)�Ƃ������΍��W)</param>
        void moveToShow(const RectF& region);
    };
}