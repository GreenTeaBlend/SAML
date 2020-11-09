#pragma once
#include <Siv3D.hpp>

namespace s3d 
{
    template <class... _Args>
    class Listener;

    /// <summary>
    /// イベントを発行する側のクラス。登録されたListenerの関数を呼び出す。
    /// </summary>
    /// <typeparam name="..._Args">イベントの引数</typeparam>
    template <class... _Args>
    class Event
    {
        template <class... _Args>
        struct ListenerPair {
            Listener<_Args...>* pListener;
            std::function<void(_Args...)>* pFunc;
        };

        Array<ListenerPair<_Args...>> m_listeners;

    public:

        Event() :
            m_listeners()
        {

        }

        void invoke(_Args... _args);

        void append(Listener<_Args...>& _listener);

        void remove(Listener<_Args...>& _listener)
        {
            for (auto it = m_listeners.begin(); it != m_listeners.end();) {
                if (it->pListener == &_listener) {
                    m_listeners.erase(it);
                    break;
                }
                else {
                    ++it;
                }
            }
            _listener.m_event = nullptr;
        }

        void operator+=(Listener<_Args...>& _listener) {
            append(_listener);
        }

        void operator-=(Listener<_Args...>& _listener) {
            remove(_listener);
        }
    };

    /// <summary>
    /// Eventクラスから通知を受け取るクラス
    /// </summary>
    /// <typeparam name="..._Args">イベントの引数</typeparam>
    template <class... _Args>
    class Listener  {
        template <class... _Args>
        friend class Event;
    protected:

        Event<_Args...>* m_event;

        std::function<void(_Args...)> m_function;
    public:
        Listener(std::function<void(_Args...)> _function) :
            m_function(_function)
        {
        }

        virtual ~Listener() {
            if (m_event != nullptr) {
                m_event->remove(*this);
                m_event = nullptr;
            }
        }
    };

    /// <summary>
    /// Eventクラスから通知を受け取り、登録されたメンバ関数を呼び出すクラス。
    /// </summary>
    template <class T, class... _Args>
    class MemberListener : public Listener<_Args...> {
        // Asset用変数
        constexpr static bool false_v = false;
        // _Argsの引数の個数
        static constexpr unsigned long long size = sizeof...(_Args);
    public:
        MemberListener(void(T::*&& _func)(_Args...), T*&& _obj) :
            Listener<_Args...>(getBind(_func, _obj))
        { }

        std::function<void(_Args...)> getBind(void(T:: * _func)(_Args...), T * _obj) {
            if constexpr (size == 0) {
                return std::bind(_func, _obj
                );
            }
            else if constexpr (size == 1) {
                return std::bind(_func, _obj,
                    std::placeholders::_1
                );
            }
            else if constexpr (size == 2) {
                return std::bind(_func, _obj,
                    std::placeholders::_1,
                    std::placeholders::_2
                );
            }
            else if constexpr (size == 3) {
                return std::bind(_func, _obj,
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
    };

    template <class... _Args>
    void Event<_Args...>::invoke(_Args... _args)
    {
        for (auto& pair : m_listeners) {
            (*pair.pFunc)(_args...);
        }
    }

    template <class... _Args>
    void Event<_Args...>::append(Listener<_Args...>& _listener)
    {
        m_listeners.push_back(ListenerPair<_Args...>{ &_listener, & _listener.m_function });
        _listener.m_event = this;
    }
}