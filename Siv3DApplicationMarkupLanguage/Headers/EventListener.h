#pragma once
#include <Siv3D.hpp>

namespace s3d {
    //template <class... _Types>
    //class Event {
    //    std::function<void(_Types...)> m_functions;
    //public:
    //    void Invoke(_Types... _Args)
    //    {
    //        m_functions(_Args...);
    //    }

    //    void append(std::function<void(_Types...)> func) {
    //        m_functions = func;
    //    }

    //    //_Ret operator()(_Types... _Args) const {
    //    //    if (_Empty()) {
    //    //        _Xbad_function_call();
    //    //    }
    //    //    const auto _Impl = _Getimpl();
    //    //    return _Impl->_Do_call(_STD forward<_Types>(_Args)...);
    //    //}
    //};

    template <class... _Types>
    class Listener {
        template <class... _Types>
        friend class Event;
    protected:

        Event<_Types...>* m_event;
        std::function<void(_Types...)> m_function;
    public:
        Listener(std::function<void(_Types...)> _function) :
            m_event(),
            m_function(_function)
        {
        }

        virtual ~Listener() {
        }
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
    class Event 
    {
        Array<ListenerPair<_Types...>> m_listeners;
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
        }

        void remove(Listener<_Types...>)
        {
            for (auto it = m_listeners.begin(); it != m_listeners.end();) {
                if (it->pObj == this) {
                    it = m_listeners.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    };
}