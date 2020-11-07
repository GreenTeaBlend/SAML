#pragma once
#include <Siv3D.hpp>

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

        SpElement m_parent;
        Array<SpElement> m_children;

        UIPanel& m_panel;

        // 子要素の追加・削除処理。
        void appendChild(const SpElement& child);
        void removeChild(const UIElement* child);

        // UIElementの情報を読み込む
        static void initialize();

        // 引数のクラス名のUIElementを生成する。
        static std::shared_ptr<UIElement> create(const String& className, UIPanel& panel, const SpElement& parent = nullptr);

        UIElement(const UIElement&) = delete;
        const UIElement& operator=(const UIElement&) = delete;

        // 描画する。
        virtual void draw() { }

    protected:

        UIElement(UIPanel& panel);

        virtual void onMouseOverChanged() {};
        virtual void onFocusChanged() {};

        // この型のプロパティ情報をdatasに追加する。
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

    public:
        ~UIElement();

        // UIの前後関係を考慮せず、マウスオーバーされているかを返す。
        virtual bool intersectsMouse() { return false; }

        // マウスオーバーされているか。UIの前後関係も考慮する。
        bool isMouseOvered() const;
        bool isFocusing() const;

        const SpElement& getParent() const { return m_parent; }

        /// <summary>
        /// このElementが属するPanel
        /// </summary>
        UIPanel& getPanel() const { return m_panel; }

        void setProperty(const String& propName, const String& value);

#if false // SamlControllerの辞書に登録する処理が必要なので、プロパティの公開はいったん後回し。
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        ///// <summary>
        ///// 指定された型のUIElementを生成し、子要素として登録する。
        ///// </summary>
        ///// <typeparam name="T"></typeparam>
        ///// <param name="parent"></param>
        ///// <returns></returns>
        //template <class T>
        //std::shared_ptr<T> createChild() {
        //    auto sp = std::shared_ptr<T>(new T(m_panel));
        //    sp->m_parent = 

        //    if (parent != nullptr) {
        //        parent->appendChild(sp);
        //    }

        //    return sp;
        //}
    };
}