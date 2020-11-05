#include "UITextIndexer.h"

using namespace s3d;
using namespace SamlUI;

UITextIndexer::UITextIndexer(const Vec2& pos, const String& text, const Font& font) :
    m_text(text),
    m_font(font),
    m_tlPos(pos),
    m_index(0),
    m_height(m_font(' ').region().h),
    m_currentRegion(pos, 0, 0)
{
    updateSize();
}

void UITextIndexer::next()
{
    if (m_text.size() <= m_index) {
        Console.writeln(U"Index out of range. m_index={}, m_text.size()={}"_fmt(m_index, m_text.size()));
    }

    const String::value_type& c = m_text.at(m_index);

    if (c == U'\n')
    {
        // 改行
        m_currentRegion.y += m_height;
        m_currentRegion.x = m_tlPos.x;
    }
    else
    {
        // 通常の文字
        m_currentRegion.x += m_font(c).region().w;
    }

    // インデックスを進める
    m_index++;

    updateSize();
}

void UITextIndexer::nextUntil(size_t index)
{
    while (m_index < index) {
        next();
    }
}

void UITextIndexer::updateSize()
{
    // 次に描画する文字の領域を取得
    const String::value_type& cNext = m_index < m_text.size() ? m_text.at(m_index) : U' ';
    m_currentRegion.size = m_font(cNext).region().size;
}