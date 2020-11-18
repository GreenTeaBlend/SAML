#pragma once
#include <Siv3D.hpp>
#include "UIElement.h"
#include "Elements/Panel.h"
#include "Components/ScrollView.h"

namespace s3d::SamlUI
{
    class ListView : public Panel
    {
    private:
        // �X�N���[���o�[�R���|�[�l���g
        ScrollView m_scrollView;

        // �X�N���[���o�[�������������̕����̕`��B�߂�l�͕\���̈�B
        SizeF drawInner(bool isMouseOvered);

    public:
        ListView();

        bool draw(bool mouseOverEnable = true) override;
    };
}