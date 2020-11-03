#include "Components/ScrollView.h"

using namespace s3d;
using namespace SamlUI;

namespace {
    const double WHEEL_SPEED = 15.0;
}

ScrollView::ScrollView() :
    m_horizontalBarThickness(20.0),
    m_verticalBarThickness(20.0),
    m_horizontalBarState(),
    m_verticalBarState()
{

}

void ScrollView::draw(std::function<s3d::SizeF()> drawInner)
{
    const RectF& rect = getRect();

    // �X�e���V����`�̐ݒ�
    RasterizerState rstatePre = Graphics2D::GetRasterizerState();
    RasterizerState rstateScissor = rstatePre;
    rstateScissor.scissorEnable = true;
    Graphics2D::Internal::SetRasterizerState(rstateScissor);

    // ������`��
    SizeF innerSize;
    {
        Vec2 offset{ m_horizontalBarState.pos * m_horizontalBarState.actualSize, m_verticalBarState.pos * m_verticalBarState.actualSize };
        Transformer2D transformer{ Mat3x2::Translate(-offset) };
        innerSize = drawInner();
    }

    // �X�N���[���o�[�̕`�攻��ƁA�e�L�X�g�`��̈�̌v�Z
    m_horizontalBarState.visible = true;
    m_verticalBarState.visible = true;
    RectF textRegion{ rect.pos, rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0) };

    // �X�e���V����`�̐ݒ��߂�
    Graphics2D::SetScissorRect(textRegion);
    Graphics2D::Internal::SetRasterizerState(rstatePre);

    // �X�N���[���o�[�̏�Ԃ̐ݒ�
    if (textRegion.w >= 0.0001 && textRegion.h >= 0.0001)
    {
        m_horizontalBarState.actualSize = innerSize.x;
        m_verticalBarState.actualSize = innerSize.y;

        m_horizontalBarState.length = innerSize.x > 1.0 ? Min(textRegion.w / innerSize.x, 1.0) : 1.0;
        m_verticalBarState.length = innerSize.y > 1.0 ? Min(textRegion.h / innerSize.y, 1.0) : 1.0;

        double wheel = WHEEL_SPEED * Mouse::Wheel();
        m_verticalBarState.pos += wheel / m_verticalBarState.actualSize;
        m_verticalBarState.pos = Clamp(m_verticalBarState.pos, 0.0, 1.0 - m_verticalBarState.length);
    }

    // �X�N���[���o�[�̕`��
    drawScrollBar();
}

void ScrollView::drawScrollBar()
{
    const RectF& rect = getRect();

    static const double PAD = 4.0;
    static const double ROUND = 2.0;

    //--------------------------------------------------
    // ���������̃o�[
    Vec2 vbarPos = rect.tr() - Vec2{ m_verticalBarThickness, 0 };
    Vec2 vbarSize = Vec2(m_verticalBarThickness, rect.h - (m_horizontalBarState.visible ? m_horizontalBarThickness : 0));
    RectF(vbarPos, vbarSize).draw(Palette::Wheat);

    // �����̃o�[
    RectF(vbarPos + PAD * Vec2::One() + Vec2{ 0, m_verticalBarState.pos * (vbarSize.y - 2 * PAD) },
        vbarSize.x - 2 * PAD,
        m_verticalBarState.length * (vbarSize.y - 2 * PAD))
        .rounded(ROUND)
        .draw(Palette::Gray);

    //--------------------------------------------------
    // ���������̃o�[
    Vec2 hbarPos = rect.bl() - Vec2{ 0, m_horizontalBarThickness };
    Vec2 hbarSize = Vec2(rect.w - (m_verticalBarState.visible ? m_verticalBarThickness : 0), m_horizontalBarThickness);
    RectF(hbarPos, hbarSize).draw(Palette::Wheat);

    // �����̃o�[
    RectF(hbarPos + PAD * Vec2::One() + Vec2{ m_horizontalBarState.pos * (hbarSize.x - 2 * PAD), 0 },
        m_horizontalBarState.length * (hbarSize.x - 2 * PAD),
        hbarSize.y - 2 * PAD)
        .rounded(ROUND)
        .draw(Palette::Gray);
}

void ScrollView::moveToShow(const RectF& region)
{
    // �[�����Z�̗}��
    if (m_horizontalBarState.actualSize < 0.0001 || m_verticalBarState.actualSize < 0.0001) {
        return;
    }

    // regionToShow�̍�����������悤�ɂȂ邽�߂̃o�[�̈ʒu�̍ŏ��l�B�ȉ��A�E�㉺�ɂ��Ă����l�B
    double leftRelativePos = region.x / m_horizontalBarState.actualSize;
    if (m_horizontalBarState.pos > leftRelativePos) { m_horizontalBarState.pos = leftRelativePos; }

    double rightRelativePos = (region.x + region.w) / m_horizontalBarState.actualSize - m_horizontalBarState.length;
    if (m_horizontalBarState.pos < rightRelativePos) { m_horizontalBarState.pos = rightRelativePos; }

    double topRelativePos = region.y / m_verticalBarState.actualSize;
    if (m_verticalBarState.pos > topRelativePos) { m_verticalBarState.pos = topRelativePos; }

    double bottomRelativePos = (region.y + region.h) / m_verticalBarState.actualSize - m_verticalBarState.length;
    if (m_verticalBarState.pos < bottomRelativePos) { m_verticalBarState.pos = bottomRelativePos; }
}