#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElementTypeInfo;
    class UIElement;
    using PropertySetter = std::function<void(s3d::SamlUI::UIElement*, const s3d::String&)>;

    // UI要素の基底クラス
    class UIElement 
    {
        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

        // 識別用のUI名
        String m_name;

    public:
        // この型のプロパティ情報をdatasに追加する。
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        // 引数のクラス名のUIElementを生成する。
        static std::shared_ptr<UIElement> create(const String& className);

#if false // SamlControllerの辞書に登録する処理が必要なので、プロパティの公開はいったん後回し。
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        // UIElementの情報を読み込む
        static void initialize();

        UIElement();

        void setProperty(const String& propName, const String& value);

        // 描画する。マウスオーバーされていたらtrueを返す。
        virtual bool draw() { return false; }

        virtual void onClicked() {}

        // マウスクリックなどによるFocus状態開始
        virtual void onFocusStart() {}
        virtual void onFocusing() {}
        virtual void onFocusEnd() {}

        virtual void onMouseOverStart() {}
        virtual void onMouseOvering() {}
        virtual void onMouseOverEnd() {}
    };
}