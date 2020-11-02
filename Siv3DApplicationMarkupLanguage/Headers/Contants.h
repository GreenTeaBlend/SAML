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
    [[nodiscard]] String ScrollVarVisibilityToString(ScrollBarVisibility);
}