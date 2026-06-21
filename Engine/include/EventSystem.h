#pragma once

#include "entt.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>

namespace Core
{
    class EventSystem
    {
    public:
        using Connection = entt::scoped_connection;

        EventSystem() = default;
        ~EventSystem() = default;

        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        EventSystem(EventSystem&&) noexcept = default;
        EventSystem& operator=(EventSystem&&) noexcept = default;

        template<typename Event, auto Handler>
        Connection Subscribe()
        {
            return Connection{ m_dispatcher.sink<Event>().template connect<Handler>() };
        }

        template<typename Event, auto Handler, typename Listener>
        Connection Subscribe(Listener& listener)
        {
            return Connection{ m_dispatcher.sink<Event>().template connect<Handler>(listener) };
        }

        template<typename Event, auto Handler, typename Listener>
        Connection Subscribe(Listener* listener)
        {
            return Connection{ m_dispatcher.sink<Event>().template connect<Handler>(listener) };
        }

        template<typename Event, typename... Args>
        void Publish(Args&&... args)
        {
            m_dispatcher.trigger<Event>(Event{ std::forward<Args>(args)... });
        }

        template<typename Event>
        void Publish(Event&& event)
        {
            m_dispatcher.trigger(std::forward<Event>(event));
        }

        template<typename Event, typename... Args>
        void Queue(Args&&... args)
        {
            m_dispatcher.enqueue<Event>(std::forward<Args>(args)...);
        }

        template<typename Event>
        void Queue(Event&& event)
        {
            m_dispatcher.enqueue(std::forward<Event>(event));
        }

        template<typename Event>
        void DispatchQueued()
        {
            m_dispatcher.update<Event>();
        }

        void DispatchQueued()
        {
            m_dispatcher.update();
        }

        template<typename Event>
        void ClearQueued()
        {
            m_dispatcher.clear<Event>();
        }

        void ClearQueued()
        {
            m_dispatcher.clear();
        }

        template<typename Listener>
        void Disconnect(Listener& listener)
        {
            m_dispatcher.disconnect(listener);
        }

        template<typename Listener>
        void Disconnect(Listener* listener)
        {
            m_dispatcher.disconnect(listener);
        }

        template<typename Event>
        std::size_t QueuedCount() const
        {
            return m_dispatcher.size<Event>();
        }

        std::size_t QueuedCount() const
        {
            return m_dispatcher.size();
        }

    private:
        entt::dispatcher m_dispatcher;
    };
}
