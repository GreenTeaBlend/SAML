#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElement;
    using SpElement = std::shared_ptr<UIElement>;

    /// <summary>
    /// UIElementインスタンスを保持するクラス。
    /// </summary>
    class UIPanel
    {
        // このPanelに属する全UIElement
        Array<std::shared_ptr<UIElement>> m_elements;

        std::shared_ptr<UIElement> m_rootElement;

        //// 引数のxmlElementをUIElementとしてParseする。isNull()==trueならfalseを返す。
        //bool parseXmlElement(XMLElement* xmlElement);
        //std::shared_ptr<UIElement> createElement(const XMLElement& xmlElement);

        // マウスオーバーしている要素 (最前面のもの)
        std::shared_ptr<UIElement> m_mouseOveredElement;

        // フォーカス中の要素
        std::shared_ptr<UIElement> m_focusingElement;

        // 識別子付きのUIElement
        s3d::HashTable<String, std::shared_ptr<UIElement>> m_namedElements;

        UIPanel(const UIPanel&) = delete;
        const UIPanel& operator=(const UIPanel&) = delete;

        UIPanel();

    public:
        /// <summary>
        /// テンプレートのUIElementをルート要素とし、UIPanelを生成する。
        /// </summary>
        /// <typeparam name="T">UIElement</typeparam>
        template <class T>
        static UIPanel* create()
        {
            auto* panel = new UIPanel();
            panel->m_rootElement = std::shared_ptr<UIElement>(new T(*panel));
            panel->m_elements.push_back(panel->m_rootElement);
            return panel;
        }

        /// <summary>
        /// XMLコードをもとにUIElementを生成する。
        /// </summary>
        static UIPanel* create(String xml);

        /// <summary>
        /// NameAttributeが設定されているUIElementを取得する。
        /// </summary>
        const std::shared_ptr<UIElement> getElement(const String& name) const {
            auto it = m_namedElements.find(name);
            return it != m_namedElements.end() ? it->second : nullptr;
        }

        /// <summary>
        /// ルート要素を取得する。
        /// </summary>
        /// <returns></returns>
        const std::shared_ptr<UIElement>& getRoot() const { return m_rootElement; }

        // 各UIElementのクリック判定などの処理と描画を行う。
        void drawUpdate();

        /// <summary>
        ///  マウスオーバーしている要素 (最前面のもの)
        /// </summary>
        const std::shared_ptr<UIElement>& getMouseOveredElement() const { return m_mouseOveredElement; }

        /// <summary>
        /// フォーカス中の要素
        /// </summary>
        const std::shared_ptr<UIElement>& getFocusingElementconst() const { return m_focusingElement; }
    };
}