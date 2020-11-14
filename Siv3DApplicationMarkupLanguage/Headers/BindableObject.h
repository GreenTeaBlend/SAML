#pragma once
#include <Siv3D.hpp>
#include "EventListener.h"

namespace s3d::SamlUI
{
    class BindableObject {

        friend class UIElement;

        struct PropertyAccessor {
            const type_info& type;
            std::shared_ptr<void> getter = nullptr;
            std::shared_ptr<void> setter = nullptr;
        };

        // ‚±‚ÌObject‚ðŠÄŽ‹‚µ‚Ä‚¢‚éUIElement
        Array<class UIElement*> m_elements;

        Array<std::pair<String, PropertyAccessor>> m_properties;

    protected:
        BindableObject();
        
       template<class T>
       //using T = int;
        void registerProperty(
            const String& name,
            const std::function<std::shared_ptr<T>()>& getter,
            const std::function<void(const T&)>& setter
        ) {
            m_properties.emplace_back(std::make_pair<String, PropertyAccessor>(
                String(name),
                PropertyAccessor{
                    typeid(T),
                    std::shared_ptr<void>(new std::function<std::shared_ptr<T>()>(getter)),
                    std::shared_ptr<void>(new std::function<void(const T&)>(setter))
                }));
        }

        void raisePropertyChanged(const String& name);

    public:

        template<class T>
        std::shared_ptr<T> getValue(const String& name) {
            for (auto& prop : m_properties) {
                if (prop.first == name)
                {
                    if (prop.second.type != typeid(T)) {
                        break;
                    }
                    void* getterPtr = prop.second.getter.get();
                    std::function<std::shared_ptr<T>()>& getterFunc = (*(std::function<std::shared_ptr<T>()>*)getterPtr);
                    return getterFunc();
                }
            }

            return nullptr;
        }

        ~BindableObject();
    };
}