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
        friend class SamlController;

        static HashTable<String, std::shared_ptr<UIElementTypeInfo>> elementDatas;

        // 識別用のUI名
        String m_name;

        bool m_isMouseOvered;
        bool m_isFocusing;

        void setMouseOvered(bool value)
        {
            if (m_isMouseOvered != value) {
                m_isMouseOvered = value;
                onMouseOverChanged();
            }
        }

        void setFocusing(bool value)
        {
            if (m_isFocusing != value) {
                m_isFocusing = value;
                onFocusChanged();
            }
        }

    protected:

        virtual void onClicked() {}
        virtual void onMouseOverChanged() {}
        virtual void onFocusChanged() {}

    public:
        // この型のプロパティ情報をdatasに追加する。
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        // 引数のクラス名のUIElementを生成する。
        static std::shared_ptr<UIElement> create(const String& className);

#if false // SamlControllerの辞書に登録する処理が必要なので、プロパティの公開はいったん後回し。
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }
#endif

        bool isMouseOvered() const { return m_isMouseOvered; }
        bool isFocusing() const { return m_isFocusing; }

        // UIElementの情報を読み込む
        static void initialize();

        UIElement();

        void setProperty(const String& propName, const String& value);

        // 描画する。マウスオーバーされていたらtrueを返す。
        virtual bool draw() { return false; }
    };
}