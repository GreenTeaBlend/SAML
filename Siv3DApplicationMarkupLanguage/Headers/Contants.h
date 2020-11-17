#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    enum class ScrollBarVisibility {
        Auto, // �K�v�ȏ�Ԃł͕\�������
        Visible, // ��ɕ\��
        Hidden // ��ɔ�\��
        //,Disabled // ��ɔ�\���ɂ��A�L�[���͂Ȃǂɂ��X�N���[�����̂��̂�����������B
    };

    [[nodiscard]] ScrollBarVisibility StringToScrollVarVisibility(const String& str);
    [[nodiscard]] String ScrollVarVisibilityToString(ScrollBarVisibility value);

    enum class HorizontalAlignment {
        Left, // ���Ɋ񂹂�
        Right, // �E�Ɋ񂹂�
        Center, // �����Ɋ񂹂� (�]�������A�T�C�Y�̂ݎQ��)
        Stretch // ���E�����ɍL���� (�T�C�Y�����A�]���̂ݎQ��)
    };

    [[nodiscard]] HorizontalAlignment StringToHorizontalAlignment(const String& str);
    [[nodiscard]] String HorizontalAlignmentToString(HorizontalAlignment value);

    enum class VerticalAlignment {
        Top, // ��Ɋ񂹂�
        Bottom, // ���Ɋ񂹂�
        Center, // �����Ɋ񂹂� (�]�������A�T�C�Y�̂ݎQ��)
        Stretch // �㉺�����ɍL���� (�T�C�Y�����A�]���̂ݎQ��)
    };

    [[nodiscard]] VerticalAlignment StringToVerticalAlignment(const String& str);
    [[nodiscard]] String VerticalAlignmentToString(VerticalAlignment value);
}