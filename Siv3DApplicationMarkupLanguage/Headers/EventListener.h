#pragma once
#include <Siv3D.hpp>

namespace s3d 
{
    template <class... _Args>
    class Listener;

    /// <summary>
    /// �C�x���g�𔭍s���鑤�̃N���X�B�o�^���ꂽListener�̊֐����Ăяo���B
    /// </summary>
    /// <typeparam name="..._Args">�C�x���g�̈���</typeparam>
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

        void invoke(_Args... _args)
        {
            for (auto& pair : m_listeners) {
                (*pair.pFunc)(_args...);
            }
        }

        /// <summary>
        /// ������Listener��ʒm��ɒǉ�����B
        /// </summary>
        void append(Listener<_Args...>& _listener)
        {
            m_listeners.push_back(ListenerPair<_Args...>{ &_listener, & _listener.m_function });
            _listener.m_event = this;
        }

        /// <summary>
        /// ������Listener��ʒm�悩��폜����B
        /// </summary>
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
    /// Event�N���X����ʒm���󂯎��A�o�^���ꂽ�֐����Ăяo���N���X�B
    /// </summary>
    /// <typeparam name="..._Args">�C�x���g�̈���</typeparam>
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
    /// Event�N���X����ʒm���󂯎��A�o�^���ꂽ�����o�֐����Ăяo���N���X�B
    /// </summary>
    template <class T, class... _Args>
    class MemberListener : public Listener<_Args...> {
        // Asset�p�ϐ�
        constexpr static bool false_v = false;
        // _Args�̈����̌�
        static constexpr unsigned long long size = sizeof...(_Args);

        // _Args�̌��ɍ��v����function��Ԃ��B
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
        /// �����o�֐���hook����Listener�𐶐�����B
        /// </summary>
        /// <param name="_func">hook�������֐�</param>
        /// <param name="_obj">hook�������I�u�W�F�N�g</param>
        MemberListener(void(T::*&& _pFunc)(_Args...), T*&& _pObj) :
            Listener<_Args...>(getBind(_pFunc, _pObj))
        { }
    };
}