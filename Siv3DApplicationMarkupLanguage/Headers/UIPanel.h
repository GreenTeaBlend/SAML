#pragma once
#include <Siv3D.hpp>

namespace s3d::SamlUI
{
    class UIElement;
    class BindableObject;

    /// <summary>
    /// UIElementインスタンスを保持するManagerクラス。
    /// </summary>
    class UIPanel
    {
        // このPanelに属する全UIElement
        Array<std::shared_ptr<UIElement>> m_elements;

        std::shared_ptr<UIElement> m_rootElement;

        // マウスオーバーしている要素 (最前面のもの)
        std::shared_ptr<UIElement> m_mouseOveredElement;

        // フォーカス中の要素
        std::shared_ptr<UIElement> m_focusingElement;

        // 描画領域(クライアント座標)
        RectF m_rect;

        // 識別子付きのUIElement
        s3d::HashTable<String, std::shared_ptr<UIElement>> m_namedElements;

        std::shared_ptr<UIElement> createElementRecursively(const XMLElement& xmlElement, const std::shared_ptr<UIElement>& parent);

        UIPanel(const UIPanel&) = delete;
        const UIPanel& operator=(const UIPanel&) = delete;

        UIPanel();

    public:
        /// <summary>
        /// テンプレートのUIElementをルート要素とし、UIPanelを生成する。
        /// </summary>
        /// <typeparam name="T">UIElement</typeparam>
        template <class T>
        static std::shared_ptr<UIPanel> create()
        {
            auto panel = std::shared_ptr<UIPanel>(new UIPanel);
            panel->m_rootElement = std::shared_ptr<UIElement>(new T(*panel));
            panel->m_elements.push_back(panel->m_rootElement);
            return panel;
        }

        /// <summary>
        /// XMLコードをもとにUIElementを生成する。
        /// </summary>
        /// <param name="error">失敗時のエラー内容</param>
        static std::shared_ptr<UIPanel> create(String xml, String* error = nullptr);

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
        const std::shared_ptr<UIElement>& getRoot() const { return m_rootElement; }

        /// <summary> 位置とサイズの基準となる矩形を取得する </summary>
        const RectF& getRect() const { return m_rect; }
        /// <summary> 位置とサイズの基準となる矩形を設定する </summary>
        void setRect(const RectF& rect);

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

        void setDataContext(BindableObject& data);
    };
}