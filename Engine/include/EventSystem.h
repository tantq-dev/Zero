#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string_view>
#include <any>

namespace Core {
	// Event data container
	struct EventData {
		std::any data;

		template<typename T>
		T get() const {
			return std::any_cast<T>(data);
		}
	};

	// Event system class
	class EventSystem {
	public:
		using EventCallback = std::function<void(const EventData&)>;

		// Singleton access
		static EventSystem& getInstance() {
			static EventSystem instance;
			return instance;
		}

		// Subscribe to an event
		void subscribe(const std::string_view& eventName, EventCallback callback) {
			m_subscribers[eventName].push_back(callback);
		}

		// Publish an event
		void publish(const std::string_view& eventName, const EventData& data) {
			auto it = m_subscribers.find(eventName);
			if (it != m_subscribers.end()) {
				for (const auto& callback : it->second) {
					callback(data);
				}
			}
		}

	private:
		EventSystem() = default;
		std::unordered_map<std::string_view, std::vector<EventCallback>> m_subscribers;
	};
}