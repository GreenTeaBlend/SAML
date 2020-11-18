#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    class Panel;

    /// <summary>
    /// UI要素の基底クラス
    /// </summary>
    class UIElement 
    {
        friend class Panel;

        class ChildrenIterator {

        };

        Panel* m_parent;

        // 他要素との余白 (右上左下)
        Vec4 m_margin;

        // サイズ設定値
        Optional<double> m_width;
        Optional<double> m_height;

        HorizontalAlignment m_horizontalAlignment;
        VerticalAlignment m_verticalAlignment;

        String m_className;

        // 最後に計算した大きさ (親要素に合わせて変化する)
        Vec2 m_currentSize;

        // 最後に計算した座標 (親要素に合わせて変化する)
        Vec2 m_currentPos;

        // 座標関連の数値の更新が必要か。
        bool m_isTransformDirty;

        std::vector<std::shared_ptr<std::function<void()>>> m_onTransformDirtyEvents;

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

    protected:

        UIElement();

        void updateTransform();

        /// <summary>
        /// WidthやHeightが設定されておらず、さらにMarginとAlignmentから要素のサイズを決定できない時に設定する大きさ。
        /// </summary>
        virtual Vec2 defaultSize() { return Vec2{ 10.0 , 10.0 }; }

    public:
        virtual ~UIElement();

#pragma region Accesor

        /// <summary> 親要素 </summary>
        Panel* getParent() const { return m_parent; }

        bool isTransformDirty() const { return m_isTransformDirty; }

        const Vec4& getMargin() const { return m_margin; }
        /// <summary> 他要素との余白を設定する。どの要素が参照されるかはAlignmentにより変わる。 </summary>
        void setMargin(const Vec4& margin)
        {
            m_margin = margin;
            setTransformDirty();
        }

        /// <summary>
        /// 幅設定値。空の値の場合、getContentHeight()のサイズをもとに幅を決める。
        /// </summary>
        const Optional<double>& getWidth() const { return m_width; }
        /// <summary>
        /// 幅設定値。空の値の場合、getContentHeight()のサイズをもとに幅を決める。
        /// </summary>
        void setWidth(const Optional<double>& width)
        {
            m_width = width;
            setTransformDirty();
        }

        /// <summary>
        /// 高さ設定値。空の値の場合、getContentHeight()のサイズをもとに高さを決める。
        /// </summary>
        const Optional<double>& getHeight() const { return m_width; }
        /// <summary>
        /// 高さ設定値。空の値の場合、getContentHeight()のサイズをもとに高さを決める。
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
        /// 親要素(もしくはウィンドウ)左上からの座標指定で位置を決める。
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
        /// 横方向の要素位置設定方法
        /// </summary>
        HorizontalAlignment getHorizontalAlignment() const { return m_horizontalAlignment; }
        /// <summary>
        /// 横方向の要素位置設定方法
        /// </summary>
        void setHorizontalAlignment(HorizontalAlignment alignment)
        {
            m_horizontalAlignment = alignment;
            setTransformDirty();
        }

        /// <summary>
        /// 縦方向の要素位置設定方法
        /// </summary>
        VerticalAlignment getVerticalAlignment() const { return m_verticalAlignment; }
        /// <summary>
        /// 縦方向の要素位置設定方法
        /// </summary>
        void setVerticalAlignment(VerticalAlignment alignment)
        {
            m_verticalAlignment = alignment;
            setTransformDirty();
        }

        /// <summary>
        /// 親要素を考慮した現在のサイズを取得する。
        /// </summary>
        const Vec2& getCurrentSize() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentSize;
        }

        /// <summary>
        /// 親要素を考慮した現在の座標(左上)を取得する。
        /// </summary>
        const Vec2& getCurrentPosition() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentPos;
        }
#pragma endregion

        /// <summary>
        /// このElementと子Elementについて、描画処理およびクリックなどのUpdate処理を再帰的に行う。
        /// </summary>
        /// <param name="mouseOverEnable">falseの場合、内部でマウスオーバーに依存する処理が行われなくなる。</param>
        /// <return>マウスオーバー判定が消費されたらtrue。これ以降の要素はマウス関連の処理がスキップされる。</return>
        virtual bool draw(bool mouseOverEnable = true) = 0;

        /// <summary>
        /// この要素(および、存在するならすべての子要素)の座標再計算フラグを立てる。
        /// </summary>
        void setTransformDirty();

        /// <summary>
        /// 座標再計算が必要になったときに呼ばれるイベントを登録する。
        /// </summary>
        void registerTransformDirtyEvent(const std::shared_ptr<std::function<void()>>& func) {
            m_onTransformDirtyEvents.push_back(func);
        }

        /// <summary>
        /// 座標再計算が必要になったときに呼ばれるイベントを解除する。
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