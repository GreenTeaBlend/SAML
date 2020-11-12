#pragma once
#include <Siv3D.hpp>
#include "Contants.h"

namespace s3d::SamlUI
{
    class UIPanel;
    class UIElementTypeInfo;
    class UIElement;
    using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;
    using SpElement = std::shared_ptr<UIElement>;

    /// <summary>
    /// UI要素の基底クラス
    /// </summary>
    class UIElement 
    {
        friend class UIPanel;

        static bool hasInitialized;

        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

        // 識別用のUI名
        String m_name;

        std::shared_ptr<UIElement> m_parent;
        Array<UIElement*> m_children;

        // 他要素との余白 (右上左下)
        Optional<Vec4> m_margin;

        // サイズ設定値 (m_marginより優先)
        Optional<Vec2> m_size;

        Optional<HorizontalAlignment> m_horizontalAlignment;
        Optional<VerticalAlignment> m_verticalAlignment;

        UIPanel& m_panel;

        String m_className;

        // 最後に計算した大きさ (親要素に合わせて変化する)
        Vec2 m_currentSize;

        // 最後に計算した座標 (親要素に合わせて変化する)
        Vec2 m_currentPos;

        // 座標関連の数値の更新が必要か。
        bool m_isTransformDirty;

        // UIElementの情報を読み込む
        static void initialize();

        // 引数のクラス名のUIElementを生成する。
        static std::shared_ptr<UIElement> create(const String& className, UIPanel& panel, const SpElement& parent);

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

        // 描画する。
        virtual void draw() { }

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

    protected:

        UIElement(UIPanel& panel);

        virtual void onMouseOverChanged() {};
        virtual void onFocusChanged() {};

        void updateTransform();

        // この要素とすべての子要素のm_isTransformDirtyをtrueにセットする。
        void setTransformDirtyRecursively();

        // この型のプロパティ情報をdatasに追加する。
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

    public:
        ~UIElement();

        // UIの前後関係を考慮せず、マウスオーバーされているかを返す。
        virtual bool intersectsMouse() { return false; }

        // マウスオーバーされているか。UIの前後関係も考慮する。
        bool isMouseOvered() const;
        bool isFocusing() const;

        /// <summary> 親要素 </summary>
        virtual void setParent(const std::shared_ptr<UIElement>& parent);
        const std::shared_ptr<UIElement>& getParent() const { return m_parent; }

        /// <summary>
        /// このElementが属するPanel
        /// </summary>
        UIPanel& getPanel() const { return m_panel; }

        void setProperty(const String& propName, const String& value);

#if false // SamlControllerの辞書に登録する処理が必要なので、プロパティの公開はいったん後回し。
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        const Array<UIElement*>& getChildren() const { return m_children; }

        bool isTransformDirty() const { return m_isTransformDirty; }

        const Optional<Vec4>& getMargin() const { return m_margin; }
        void setMargin(const Optional<Vec4>& margin);

        const Optional<Vec2>& getSize() const { return m_size; }
        void setSize(const Optional<Vec2>& size);

        const Optional<HorizontalAlignment>& getHorizontalAlignment() const { return m_horizontalAlignment; }
        void setHorizontalAlignment(const Optional<HorizontalAlignment>& alignment);

        const Optional<VerticalAlignment>& getVerticalAlignment() const { return m_verticalAlignment; }
        void setVerticalAlignment(const Optional<VerticalAlignment>& alignment);

        const Vec2& getSize() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentSize;
        }

        const Vec2& getPosition() {
            if (isTransformDirty()) {
                updateTransform();
            }
            return m_currentPos;
        }
    };
}