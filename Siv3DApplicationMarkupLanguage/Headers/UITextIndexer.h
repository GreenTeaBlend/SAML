#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    /// <summary>
    /// �����񒆂̕������Ƃ̕`��̈�����J��ɕԂ��N���X
    /// </summary>
    class UITextIndexer
    {
        const String& m_text;
        const Font& m_font;

        // �S�̂̍�����W
        const Vec2 m_tlPos;
        // �����̍���
        const double m_height;

        // ���݂̕����̕`��̈�
        RectF m_currentRegion;

        size_t m_index;

        // ���ɕ`�悷�镶����size���X�V
        void updateSize();

    public:
        UITextIndexer(const Vec2& pos, const String& text, const Font& font);

        // ���̕����Ɉړ�����B
        void next();

        // �w�肳�ꂽIndex�ɂȂ�܂œ������B index <= currentIndex() �̏ꍇ�͉������Ȃ��B
        void nextUntil(size_t index);

        // index��������̊O�̏ꍇfalse�ƂȂ�B
        bool isValid() const { return m_index < m_text.size(); }

        // ��������ł̌��݂̗v�f�ԍ��B
        const size_t& currentIndex() const { return m_index; }

        // ���݂̕���
        const String::value_type& currentChar() const { return m_text.at(m_index); }

        // ���݂̕����̗̈� (isValid()==false�̏ꍇ�Asize�͔��p�X�y�[�X�̐��l)
        const RectF& currentRegion() const { return m_currentRegion; }

        // ���݂̕����̃t�H���g (�����̃t�H���g�ɑΉ��\��)
        const Font& currentFont() const { return m_font; }

        // ���݂̍s�̍��� == ���ɉ��s����Ƃ���y���W�ɉ��Z�����
        double currentHeight() const { return m_height; }
    };
}