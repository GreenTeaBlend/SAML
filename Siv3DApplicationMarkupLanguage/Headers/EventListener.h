#pragma once
#include <Siv3D.hpp>

namespace s3d 
{
    template <class... _Args>
    class Listener;

    /// <summary>
    /// イベント通知クラス。登録されたListenerや関数を呼び出す。
    /// </summary>
    /// <typeparam name="..._Args">イベントの引数</typeparam>
    template <class... _Args>
    class Event
    {
        template <class... _Args>
        struct ListenerPair {
            // pFuncを保持しているEvent。
            Listener<_Args...>* pListener;
            // コールバック関数。
            std::shared_ptr<std::function<void(_Args...)>> pFunc;
        };

        Array<ListenerPair<_Args...>> m_listeners;

    public:

        Event() :
            m_listeners()
        {
        }

        ~Event()
        {
            // 自身に登録しているすべてのListenerのm_eventsから自身を削除
            for (auto& listenerPair : m_listeners) {
                auto* pListener = listenerPair.pListener;
                if (pListener != nullptr) {
                    for (auto it = pListener->m_events.begin(); it != pListener->m_events.end(); ++it) {
                        if (*it == this) {
                            pListener->m_events.erase(it);
                            break;
                        }
                    }
                }
            }

            // 自身に登録しているListenerをクリア
            m_listeners.clear();
        }

        /// <summary>
        /// イベントの通知を実行する
        /// </summary>
        void invoke(_Args... _args)
        {
            for (auto& pair : m_listeners) {
                (*pair.pFunc)(_args...);
            }
        }

        /// <summary>
        /// 引数のListenerを通知先に追加する。
        /// </summary>
        void append(Listener<_Args...>& _listener)
        {
            m_listeners.push_back(ListenerPair<_Args...>{ &_listener, _listener.m_function });
            _listener.m_events.push_back(this);
        }

        /// <summary>
        /// 引数のListenerを通知先から削除する。
        /// </summary>
        void remove(Listener<_Args...>& _listener)
        {
            // m_listeners から _listener に関する要素を削除する
            for (auto it = m_listeners.begin(); ; ++it) {
                if (it == m_listeners.end()) {
#if _DEBUG
                    // デバッグモードの場合、削除する要素が見つからなければエラーを流す。
                    Console.writeln(U"Failed to remove the listener from the event.");
#endif
                    break;
                }
                else if (it->pListener == &_listener) {
                    m_listeners.erase(it);
                    break;
                }
            }

            // _listener.m_eventsから自身を削除
            for (auto it = _listener.m_events.begin(); it != _listener.m_events.end(); ++it ) {
                if (*it == this) {
                    _listener.m_events.erase(it);
                    break;
                }
            }
        }

        /// <summary>
        /// Listenerをhookする。
        /// </summary>
        void operator+=(Listener<_Args...>& _listener) {
            append(_listener);
        }

        /// <summary>
        /// Listenerをunhookする。
        /// </summary>
        void operator-=(Listener<_Args...>& _listener) {
            remove(_listener);
        }

        /// <summary>
        /// function/ラムダ式を直接hookする。(unhook不可)
        /// </summary>
        void operator+=(const std::function<void(_Args...)>& _function) {
            auto spFunc = std::make_shared<std::function<void(_Args...)>>(_function);
            m_listeners.push_back(ListenerPair<_Args...>{ nullptr, spFunc });
        }
    };

    /// <summary>
    /// Eventクラスから通知を受け取り、登録された関数を呼び出すクラス。
    /// Eventからremoveするか、このインスタンスが削除された際にunhookする。
    /// </summary>
    /// <typeparam name="..._Args">イベントの引数</typeparam>
    template <class... _Args>
    class Listener  {
        template <class... _Args>
        friend class Event;
    protected:
        // このListenerがhookしているEvent
        Array<Event<_Args...>*> m_events;

        std::shared_ptr<std::function<void(_Args...)>> m_function;
    public:
        /// <summary>
        /// 任意の関数をhookするListenerを生成する。
        /// </summary>
        Listener(const std::function<void(_Args...)>& _function) :
            m_events(),
            m_function(new std::function<void(_Args...)>(_function))
        {
        }

        virtual ~Listener() {
            // 自身が登録しているすべてのEventから自身をunhookする。
            // Event::remove()の内部でm_eventsの要素の削除が行われるので、remove()だけ呼んで回す。
            while (m_events.size() != 0) {
                m_events.back()->remove(*this);
            }
        }
    };

    /// <summary>
    /// Eventクラスから通知を受け取り、登録されたメンバ関数を呼び出すクラス。
    /// Eventからremoveするか、このインスタンスが削除された際にunhookする。
    /// </summary>
    template <class T, class... _Args>
    class MemberListener : public Listener<_Args...> {
        // Asset用変数
        constexpr static bool false_v = false;
        // _Argsの引数の個数
        static constexpr unsigned long long size = sizeof...(_Args);

        // _Argsの個数に合致するfunctionを返す。
        std::function<void(_Args...)> getBind(void(T::* _pFunc)(_Args...), T* _pObj) {
            if constexpr (size == 0) {
                return std::bind(_pFunc, _pObj
                );
            }
            else if constexpr (size == 1) {
                return std::bind(_pFunc, _pObj,
                    std::placeholders::_1
                );
            }
            else if constexpr (size == 2) {
                return std::bind(_pFunc, _pObj,
                    std::placeholders::_1,
                    std::placeholders::_2
                );
            }
            else if constexpr (size == 3) {
                return std::bind(_pFunc, _pObj,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                );
            }
            else {
                static_assert(false_v, "MemberListener does not expect more than 4 arguments.");
                return nullptr;
            }
        }
    public:
        /// <summary>
        /// メンバ関数をhookするListenerを生成する。
        /// </summary>
        /// <param name="_func">hookしたい関数</param>
        /// <param name="_obj">hookしたいオブジェクト</param>
        MemberListener(void(T::*&& _pFunc)(_Args...), T*&& _pObj) :
            Listener<_Args...>(getBind(_pFunc, _pObj))
        { }
    };
}