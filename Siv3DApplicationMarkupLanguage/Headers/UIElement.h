#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    /// <summary>
    /// UI要素の基底クラス
    /// </summary>
    class UIElement 
    {
        class ChildrenIterator {

        };

        UIElement* m_parent;
        Array<UIElement*> m_children;

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

        // マウスオーバーされているか(UIの前後関係も考慮)
        bool m_isMouseOvered;

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

#if 0

        // UIElementの情報を読み込む
        static void initialize();

        static bool hasInitialized;

        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

        // 識別用のUI名
        String m_name;

        UIPanel& m_panel;

        BindableObject* m_dataContext;

        // 引数のクラス名のUIElementを生成する。
        static std::shared_ptr<UIElement> create(const String& className, UIPanel& panel, const SpElement& parent);

        // このインスタンスのクラス名を返す。
        String className() {
            if (m_className.length() == 0) {
                // "class Saml::UIElement::Button" のような文字列が取得されるので、名前空間も除いた純粋なクラス名に変換する。
                m_className = Unicode::Widen(typeid(*this).name());
                auto colonIndex = m_className.lastIndexOf(U":");
                if (colonIndex != String::npos) {
                    m_className = m_className.substr(colonIndex + 1); // コロンより後を取得
                }
                else {
                    m_className = m_className.substr(6); // 先頭の"class "より後を取得
                }
            }
            return m_className;
        }
#endif

        // このオブジェクトと子を再帰的に描画する。
        void drawRecursively(bool& mouseOverDisabled);

    protected:

        UIElement();

        void updateTransform();

        //// この型のプロパティ情報をdatasに追加する。
        //static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        /// <summary>
        /// 描画する。
        /// </summary>
        virtual void onDraw() = 0;

    public:
        ~UIElement();

#pragma region Accesor

        // UIの前後関係を考慮せず、マウスオーバーされているかを返す。
        virtual bool intersectsMouse() { return RectF{ getCurrentPosition(), getCurrentSize() }.mouseOver(); }

        // マウスオーバーされているか。UIの前後関係も考慮する。
        bool isMouseOvered() const { return m_isMouseOvered; }

        /// <summary> 親要素 </summary>
        void setParent(UIElement* parent);
        UIElement* getParent() const { return m_parent; }

        const Array<UIElement*> getChildren() const { return m_children; }

        bool isTransformDirty() const { return m_isTransformDirty; }

        const Vec4& getMargin() const { return m_margin; }
        /// <summary> 他要素との余白を設定する。どの要素が参照されるかはAlignmentにより変わる。 </summary>
        void setMargin(const Vec4& margin)
        {
            m_margin = margin;
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
        }

        void setSize(const Vec2& size)
        {
            m_width = size.x;
            m_height = size.y;
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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
            setTransformDirtyRecursively();
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

        /// <summary>
        /// 内部要素の大きさ(ボタンの中の文字等)
        /// </summary>
        virtual Vec2 getContentSize() const { return Vec2::Zero(); }
#pragma endregion

        /// <summary>
        /// このElementと子Elementについて、描画処理およびクリックなどのUpdate処理を再帰的に行う。
        /// </summary>
        void draw();

        /// <summary>
        /// この要素とすべての子要素の座標再計算フラグを立てる。
        /// </summary>
        void setTransformDirtyRecursively();
    };
}