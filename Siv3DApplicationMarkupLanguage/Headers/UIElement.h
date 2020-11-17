#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    /// <summary>
    /// UI�v�f�̊��N���X
    /// </summary>
    class UIElement 
    {
        class ChildrenIterator {

        };

        UIElement* m_parent;
        Array<UIElement*> m_children;

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

        // �}�E�X�I�[�o�[����Ă��邩(UI�̑O��֌W���l��)
        bool m_isMouseOvered;

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

#if 0

        // UIElement�̏���ǂݍ���
        static void initialize();

        static bool hasInitialized;

        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

        // ���ʗp��UI��
        String m_name;

        UIPanel& m_panel;

        BindableObject* m_dataContext;

        // �����̃N���X����UIElement�𐶐�����B
        static std::shared_ptr<UIElement> create(const String& className, UIPanel& panel, const SpElement& parent);

        // ���̃C���X�^���X�̃N���X����Ԃ��B
        String className() {
            if (m_className.length() == 0) {
                // "class Saml::UIElement::Button" �̂悤�ȕ����񂪎擾�����̂ŁA���O��Ԃ������������ȃN���X���ɕϊ�����B
                m_className = Unicode::Widen(typeid(*this).name());
                auto colonIndex = m_className.lastIndexOf(U":");
                if (colonIndex != String::npos) {
                    m_className = m_className.substr(colonIndex + 1); // �R����������擾
                }
                else {
                    m_className = m_className.substr(6); // �擪��"class "������擾
                }
            }
            return m_className;
        }
#endif

        // ���̃I�u�W�F�N�g�Ǝq���ċA�I�ɕ`�悷��B
        void drawRecursively(bool& mouseOverDisabled);

    protected:

        UIElement();

        void updateTransform();

        //// ���̌^�̃v���p�e�B����datas�ɒǉ�����B
        //static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        /// <summary>
        /// �`�悷��B
        /// </summary>
        virtual void onDraw() = 0;

    public:
        ~UIElement();

#pragma region Accesor

        // UI�̑O��֌W���l�������A�}�E�X�I�[�o�[����Ă��邩��Ԃ��B
        virtual bool intersectsMouse() { return RectF{ getCurrentPosition(), getCurrentSize() }.mouseOver(); }

        // �}�E�X�I�[�o�[����Ă��邩�BUI�̑O��֌W���l������B
        bool isMouseOvered() const { return m_isMouseOvered; }

        /// <summary> �e�v�f </summary>
        void setParent(UIElement* parent);
        UIElement* getParent() const { return m_parent; }

        const Array<UIElement*> getChildren() const { return m_children; }

        bool isTransformDirty() const { return m_isTransformDirty; }

        const Vec4& getMargin() const { return m_margin; }
        /// <summary> ���v�f�Ƃ̗]����ݒ肷��B�ǂ̗v�f���Q�Ƃ���邩��Alignment�ɂ��ς��B </summary>
        void setMargin(const Vec4& margin)
        {
            m_margin = margin;
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
        }

        void setSize(const Vec2& size)
        {
            m_width = size.x;
            m_height = size.y;
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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

        /// <summary>
        /// �����v�f�̑傫��(�{�^���̒��̕�����)
        /// </summary>
        virtual Vec2 getContentSize() const { return Vec2::Zero(); }
#pragma endregion

        /// <summary>
        /// ����Element�ƎqElement�ɂ��āA�`�揈������уN���b�N�Ȃǂ�Update�������ċA�I�ɍs���B
        /// </summary>
        void draw();

        /// <summary>
        /// ���̗v�f�Ƃ��ׂĂ̎q�v�f�̍��W�Čv�Z�t���O�𗧂Ă�B
        /// </summary>
        void setTransformDirtyRecursively();
    };
}