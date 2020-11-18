#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    class Panel;

    /// <summary>
    /// UI�v�f�̊��N���X
    /// </summary>
    class UIElement 
    {
        friend class Panel;

        class ChildrenIterator {

        };

        Panel* m_parent;

        // ���v�f�Ƃ̗]�� (�E�㍶��)
        Vec4 m_margin;

        // �T�C�Y�ݒ�l
        Optional<double> m_width;
        Optional<double> m_height;

        HorizontalAlignment m_horizontalAlignment;
        VerticalAlignment m_verticalAlignment;

        String m_className;

        // �Ō�Ɍv�Z�����傫�� (�e�v�f�ɍ��킹�ĕω�����)
        Vec2 m_currentSize;

        // �Ō�Ɍv�Z�������W (�e�v�f�ɍ��킹�ĕω�����)
        Vec2 m_currentPos;

        // ���W�֘A�̐��l�̍X�V���K�v���B
        bool m_isTransformDirty;

        std::vector<std::shared_ptr<std::function<void()>>> m_onTransformDirtyEvents;

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

    protected:

        UIElement();

        void updateTransform();

        /// <summary>
        /// Width��Height���ݒ肳��Ă��炸�A�����Margin��Alignment����v�f�̃T�C�Y������ł��Ȃ����ɐݒ肷��傫���B
        /// </summary>
        virtual Vec2 defaultSize() { return Vec2{ 10.0 , 10.0 }; }

    public:
        virtual ~UIElement();

#pragma region Accesor

        /// <summary> �e�v�f </summary>
        Panel* getParent() const { return m_parent; }

        bool isTransformDirty() const { return m_isTransformDirty; }

        const Vec4& getMargin() const { return m_margin; }
        /// <summary> ���v�f�Ƃ̗]����ݒ肷��B�ǂ̗v�f���Q�Ƃ���邩��Alignment�ɂ��ς��B </summary>
        void setMargin(const Vec4& margin)
        {
            m_margin = margin;
            setTransformDirty();
        }

        /// <summary>
        /// ���ݒ�l�B��̒l�̏ꍇ�AgetContentHeight()�̃T�C�Y�����Ƃɕ������߂�B
        /// </summary>
        const Optional<double>& getWidth() const { return m_width; }
        /// <summary>
        /// ���ݒ�l�B��̒l�̏ꍇ�AgetContentHeight()�̃T�C�Y�����Ƃɕ������߂�B
        /// </summary>
        void setWidth(const Optional<double>& width)
        {
            m_width = width;
            setTransformDirty();
        }

        /// <summary>
        /// �����ݒ�l�B��̒l�̏ꍇ�AgetContentHeight()�̃T�C�Y�����Ƃɍ��������߂�B
        /// </summary>
        const Optional<double>& getHeight() const { return m_width; }
        /// <summary>
        /// �����ݒ�l�B��̒l�̏ꍇ�AgetContentHeight()�̃T�C�Y�����Ƃɍ��������߂�B
        /// </summary>
        void setHeight(const Optional<double>& height)
        {
            m_height = height;
            setTransformDirty();
        }

        void setSize(const Vec2& size)
        {
            m_width = size.x;
            m_height = size.y;
            setTransformDirty();
        }

        /// <summary>
        /// �e�v�f(�������̓E�B���h�E)���ォ��̍��W�w��ňʒu�����߂�B
        /// </summary>
        void setAsRect(const RectF& rect)
        {
            m_horizontalAlignment = HorizontalAlignment::Left;
            m_verticalAlignment = VerticalAlignment::Top;
            m_width = rect.w;
            m_height = rect.h;
            m_margin = Vec4(rect.pos, Vec2::Zero());
            setTransformDirty();
        }

        /// <summary>
        /// �������̗v�f�ʒu�ݒ���@
        /// </summary>
        HorizontalAlignment getHorizontalAlignment() const { return m_horizontalAlignment; }
        /// <summary>
        /// �������̗v�f�ʒu�ݒ���@
        /// </summary>
        void setHorizontalAlignment(HorizontalAlignment alignment)
        {
            m_horizontalAlignment = alignment;
            setTransformDirty();
        }

        /// <summary>
        /// �c�����̗v�f�ʒu�ݒ���@
        /// </summary>
        VerticalAlignment getVerticalAlignment() const { return m_verticalAlignment; }
        /// <summary>
        /// �c�����̗v�f�ʒu�ݒ���@
        /// </summary>
        void setVerticalAlignment(VerticalAlignment alignment)
        {
            m_verticalAlignment = alignment;
            setTransformDirty();
        }

        /// <summary>
        /// �e�v�f���l���������݂̃T�C�Y���擾����B
        /// </summary>
        const Vec2& getCurrentSize() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentSize;
        }

        /// <summary>
        /// �e�v�f���l���������݂̍��W(����)���擾����B
        /// </summary>
        const Vec2& getCurrentPosition() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentPos;
        }
#pragma endregion

        /// <summary>
        /// ����Element�ƎqElement�ɂ��āA�`�揈������уN���b�N�Ȃǂ�Update�������ċA�I�ɍs���B
        /// </summary>
        /// <param name="mouseOverEnable">false�̏ꍇ�A�����Ń}�E�X�I�[�o�[�Ɉˑ����鏈�����s���Ȃ��Ȃ�B</param>
        /// <return>�}�E�X�I�[�o�[���肪����ꂽ��true�B����ȍ~�̗v�f�̓}�E�X�֘A�̏������X�L�b�v�����B</return>
        virtual bool draw(bool mouseOverEnable = true) = 0;

        /// <summary>
        /// ���̗v�f(����сA���݂���Ȃ炷�ׂĂ̎q�v�f)�̍��W�Čv�Z�t���O�𗧂Ă�B
        /// </summary>
        void setTransformDirty();

        /// <summary>
        /// ���W�Čv�Z���K�v�ɂȂ����Ƃ��ɌĂ΂��C�x���g��o�^����B
        /// </summary>
        void registerTransformDirtyEvent(const std::shared_ptr<std::function<void()>>& func) {
            m_onTransformDirtyEvents.push_back(func);
        }

        /// <summary>
        /// ���W�Čv�Z���K�v�ɂȂ����Ƃ��ɌĂ΂��C�x���g����������B
        /// </summary>
        void removeTransformDirtyEvent(const std::shared_ptr<std::function<void()>>& func) {
            for (auto it = m_onTransformDirtyEvents.begin(); it != m_onTransformDirtyEvents.end(); it++) {
                if ((*it) == func) {
                    m_onTransformDirtyEvents.erase(it);
                    return;
                }
            }
        }
    };
}