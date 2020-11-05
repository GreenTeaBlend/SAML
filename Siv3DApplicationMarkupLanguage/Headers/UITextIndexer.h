#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    /// <summary>
    /// 文字列中の文字ごとの描画領域を順繰りに返すクラス
    /// </summary>
    class UITextIndexer
    {
        const String& m_text;
        const Font& m_font;

        // 全体の左上座標
        const Vec2 m_tlPos;
        // 文字の高さ
        const double m_height;

        // 現在の文字の描画領域
        RectF m_currentRegion;

        size_t m_index;

        // 次に描画する文字のsizeを更新
        void updateSize();

    public:
        UITextIndexer(const Vec2& pos, const String& text, const Font& font);

        // 次の文字に移動する。
        void next();

        // 指定されたIndexになるまで動かす。 index <= currentIndex() の場合は何もしない。
        void nextUntil(size_t index);

        // indexが文字列の外の場合falseとなる。
        bool isValid() const { return m_index < m_text.size(); }

        // 文字列内での現在の要素番号。
        const size_t& currentIndex() const { return m_index; }

        // 現在の文字
        const String::value_type& currentChar() const { return m_text.at(m_index); }

        // 現在の文字の領域 (isValid()==falseの場合、sizeは半角スペースの数値)
        const RectF& currentRegion() const { return m_currentRegion; }

        // 現在の文字のフォント (複数のフォントに対応予定)
        const Font& currentFont() const { return m_font; }

        // 現在の行の高さ == 次に改行するときのy座標に加算する量
        double currentHeight() const { return m_height; }
    };
}