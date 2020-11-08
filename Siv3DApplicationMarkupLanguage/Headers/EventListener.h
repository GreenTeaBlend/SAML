#pragma once
#include <Siv3D.hpp>

namespace s3d 
{
    class EventBase;

    class ListenerBase {
        template <class... _Types>
        friend class Event;
        friend class EventBase;
    protected:

        EventBase* m_event;
    public:
        ListenerBase() :
            m_event()
        {
        }

        virtual ~ListenerBase();
    };

    class EventBase 
    {
    public:
        virtual ~EventBase() = default;
        virtual void remove(ListenerBase& _pKey) = 0;
    };

    template <class... _Types>
    class Listener : public ListenerBase {
        template <class... _Types>
        friend class Event;
    protected:

        std::function<void(_Types...)> m_function;
    public:
        Listener(std::function<void(_Types...)> _function) :
            m_function(_function)
        {
        }

        virtual ~Listener() = default;
    };

    template <class T, class _Arg1>
    class MemberListener : public Listener<_Arg1> {
    public:
        MemberListener(void(T::*&& _func)(_Arg1), T*&& _obj) :
            Listener<_Arg1>(std::bind(_func, _obj,
                std::placeholders::_1))
        { }
    };

    template <class... _Types>
    struct ListenerPair {
        void* pObj;
        std::function<void(_Types...)>* pFunc;
    };

    template <class... _Types>
    class Event : public EventBase
    {
        Array<ListenerPair<_Types...>> m_listeners;

    protected:

    public:

        Event() :
            m_listeners()
        {

        }

        void invoke(_Types... _Args)
        {
            for (auto& pair : m_listeners) {
                (*pair.pFunc)(_Args...);
            }
        }

        void append(Listener<_Types...>& _listener)
        {
            m_listeners.push_back(ListenerPair<_Types...>{ &_listener, &_listener.m_function });
            _listener.m_event = this;
        }

        void remove(ListenerBase& _listener) override
        {
            for (auto it = m_listeners.begin(); it != m_listeners.end();) {
                if (it->pObj == &_listener) {
                    m_listeners.erase(it);
                    break;
                }
                else {
                    ++it;
                }
            }
            _listener.m_event = nullptr;
        }

        void operator+=(Listener<_Types...>& _listener) {
            append(_listener);
        }

        void operator-=(Listener<_Types...>& _listener) {
            remove(_listener);
        }
    };
}