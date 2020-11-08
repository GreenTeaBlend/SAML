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
    struct ListenerPair {
        void* pObj;
        std::function<void(_Types...)>* pFunc;
    };

    template <class... _Types>
    class Event 
    {
        template <class... _Types>
        friend class Listener;

        Array<ListenerPair<_Types...>> m_listeners;
    public:
        Event() :
            m_listeners()
        {

        }

        void Invoke(_Types... _Args)
        {
            for (auto& pair : m_listeners) {
                (*pair.pFunc)(_Args...);
            }
            //m_functions(_Args...);
        }

        //void append(std::function<void(_Types...)> func) {
        //    m_functions = func;
        //}

        //template<class T>
        //void append(void(T::*&&_func)(_Types... _Args)) {
        //    std::function<int(int, int, int)> FuncOfHoge = std::bind(
        //        &Hoge::fuga,
        //        &hoge,
        //        std::placeholders::_1,
        //        std::placeholders::_2,
        //        std::placeholders::_3);
        //}
    };

    template <class... _Types>
    class Listener {
    protected:
        Event<_Types...>& m_event;
        std::function<void(_Types...)> m_function;
    public:
        Listener(Event<_Types...>& _event, std::function<void(_Types...)> _function) :
            m_event(_event),
            m_function(_function)
        {
            m_event.m_listeners.push_back(ListenerPair<_Types...>{ this, &m_function });
        }

        virtual ~Listener() {
            for (auto it = m_event.m_listeners.begin(); it != m_event.m_listeners.end();) {
                if (it->pObj == this) {
                    it = m_event.m_listeners.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    };

    //template <class T, class _Arg1>
    //class BindListener : public Listener<_Arg1> {
    //public:
    //    BindListener(Event<_Arg1>& _event, void(T::*&& _func)(_Arg1), T*&& _obj) :
    //        Listener<_Arg1>(_event, /*std::bind(
    //            _func,
    //            _obj,
    //            std::placeholders::_1)*/[&](int a) {Console.writeln(U"{}"_fmt(a)); })
    //    {

    //    }
    //};
}