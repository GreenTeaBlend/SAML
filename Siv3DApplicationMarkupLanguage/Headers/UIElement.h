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

    public:
        // この型のプロパティ情報をdatasに追加する。
        static void enumratePropertyData(HashTable<String, PropertySetter>* datas);

        // 引数のクラス名のUIElementを生成する。
        static std::shared_ptr<UIElement> create(const String& className);

        // UIElementの情報を読み込む
        static void initialize();

        UIElement();

        void setProperty(const String& propName, const String& value);

        virtual void draw() {};
    };
}