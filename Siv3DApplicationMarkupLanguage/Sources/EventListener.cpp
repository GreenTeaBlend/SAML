#include "EventListener.h"

using namespace s3d;

ListenerBase::~ListenerBase() {
    if (m_event != nullptr) {
        m_event->remove(*this);
        m_event = nullptr;
    }
}