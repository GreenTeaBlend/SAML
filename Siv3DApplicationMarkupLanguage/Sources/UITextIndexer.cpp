#include "UITextIndexer.h"

using namespace s3d;
using namespace SamlUI;

UITextIndexer::UITextIndexer(const Vec2& pos, const String& text, const Font& font) :
    m_text(text),
    m_font(font),
    m_tlPos(pos),
    m_index(0),
    m_height(m_font(' ').region().h),
    m_currentRegion(pos, 0, 0),
    m_currenMaxHeight(-1.0)
{
    // Ÿ‚É•`‰æ‚·‚é•¶š‚Ì—Ìˆæ‚ğæ“¾
    const String::value_type& cNext = m_index < m_text.size() ? m_text.at(m_index) : U' ';
    m_currentRegion.size = m_font(cNext).region().size;
}

void UITextIndexer::next()
{
    const String::value_type& c = m_text.at(m_index);

    if (c == U'\n')
    {
        m_currentRegion.y += m_currenMaxHeight > 0 ? m_currenMaxHeight : m_height;
        m_currentRegion.x = m_tlPos.x;
    }
    else
    {
        RectF charRect = m_font(c).region(m_currentRegion.pos);
        m_currentRegion.x = charRect.br().x;

        m_currenMaxHeight = Max(m_currenMaxHeight, m_height);
    }

    // ”Ô†‚ği‚ß‚é
    m_index++;

    // Ÿ‚É•`‰æ‚·‚é•¶š‚Ì—Ìˆæ‚ğæ“¾
    const String::value_type& cNext = m_index < m_text.size() ? m_text.at(m_index) : U' ';
    m_currentRegion.size = m_font(cNext).region().size;
}