#pragma once
#include <Siv3D.hpp>

namespace s3d 
{
    template <class... _Args>
    class Listener;

    /// <summary>
    /// �C�x���g�ʒm�N���X�B�o�^���ꂽListener��֐����Ăяo���B
    /// </summary>
    /// <typeparam name="..._Args">�C�x���g�̈���</typeparam>
    template <class... _Args>
    class Event
    {
        template <class... _Args>
        struct ListenerPair {
            // pFunc��ێ����Ă���Event�B
            Listener<_Args...>* pListener;
            // �R�[���o�b�N�֐��B
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
            // ���g�ɓo�^���Ă��邷�ׂĂ�Listener��m_events���玩�g���폜
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

            // ���g�ɓo�^���Ă���Listener���N���A
            m_listeners.clear();
        }

        /// <summary>
        /// �C�x���g�̒ʒm�����s����
        /// </summary>
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
            m_listeners.push_back(ListenerPair<_Args...>{ &_listener, _listener.m_function });
            _listener.m_events.push_back(this);
        }

        /// <summary>
        /// ������Listener��ʒm�悩��폜����B
        /// </summary>
        void remove(Listener<_Args...>& _listener)
        {
            // m_listeners ���� _listener �Ɋւ���v�f���폜����
            for (auto it = m_listeners.begin(); ; ++it) {
                if (it == m_listeners.end()) {
#if _DEBUG
                    // �f�o�b�O���[�h�̏ꍇ�A�폜����v�f��������Ȃ���΃G���[�𗬂��B
                    Console.writeln(U"Failed to remove the listener from the event.");
#endif
                    break;
                }
                else if (it->pListener == &_listener) {
                    m_listeners.erase(it);
                    break;
                }
            }

            // _listener.m_events���玩�g���폜
            for (auto it = _listener.m_events.begin(); it != _listener.m_events.end(); ++it ) {
                if (*it == this) {
                    _listener.m_events.erase(it);
                    break;
                }
            }
        }

        /// <summary>
        /// Listener��hook����B
        /// </summary>
        void operator+=(Listener<_Args...>& _listener) {
            append(_listener);
        }

        /// <summary>
        /// Listener��unhook����B
        /// </summary>
        void operator-=(Listener<_Args...>& _listener) {
            remove(_listener);
        }

        /// <summary>
        /// function/�����_���𒼐�hook����B(unhook�s��)
        /// </summary>
        void operator+=(const std::function<void(_Args...)>& _function) {
            auto spFunc = std::make_shared<std::function<void(_Args...)>>(_function);
            m_listeners.push_back(ListenerPair<_Args...>{ nullptr, spFunc });
        }
    };

    /// <summary>
    /// Event�N���X����ʒm���󂯎��A�o�^���ꂽ�֐����Ăяo���N���X�B
    /// Event����remove���邩�A���̃C���X�^���X���폜���ꂽ�ۂ�unhook����B
    /// </summary>
    /// <typeparam name="..._Args">�C�x���g�̈���</typeparam>
    template <class... _Args>
    class Listener  {
        template <class... _Args>
        friend class Event;
    protected:
        // ����Listener��hook���Ă���Event
        Array<Event<_Args...>*> m_events;

        std::shared_ptr<std::function<void(_Args...)>> m_function;
    public:
        /// <summary>
        /// �C�ӂ̊֐���hook����Listener�𐶐�����B
        /// </summary>
        Listener(const std::function<void(_Args...)>& _function) :
            m_events(),
            m_function(new std::function<void(_Args...)>(_function))
        {
        }

        virtual ~Listener() {
            // ���g���o�^���Ă��邷�ׂĂ�Event���玩�g��unhook����B
            // Event::remove()�̓�����m_events�̗v�f�̍폜���s����̂ŁAremove()�����Ă�ŉ񂷁B
            while (m_events.size() != 0) {
                m_events.back()->remove(*this);
            }
        }
    };

    /// <summary>
    /// Event�N���X����ʒm���󂯎��A�o�^���ꂽ�����o�֐����Ăяo���N���X�B
    /// Event����remove���邩�A���̃C���X�^���X���폜���ꂽ�ۂ�unhook����B
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