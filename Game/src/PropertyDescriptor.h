#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <imgui.h>

namespace Tool::UI {

	/**
	 * Base class for UI property descriptors
	 */
	class PropertyDescriptor {
	public:
		PropertyDescriptor(const std::string& name, const std::string& tooltip = "")
			: m_name(name), m_tooltip(tooltip) {
		}

		virtual ~PropertyDescriptor() = default;

		// Renders the property UI and returns true if value changed
		virtual bool RenderImGui() = 0;

	protected:
		std::string m_name;
		std::string m_tooltip;

		// Helper to display tooltip
		void ShowTooltipIfHovered() const {
			if (!m_tooltip.empty() && ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::TextUnformatted(m_tooltip.c_str());
				ImGui::EndTooltip();
			}
		}
	};

	/**
	 * Integer property descriptor
	 */
	class IntPropertyDescriptor : public PropertyDescriptor {
	public:
		IntPropertyDescriptor(const std::string& name, int* valuePtr,
			int minValue = 0, int maxValue = INT_MAX,
			const std::string& tooltip = "")
			: PropertyDescriptor(name, tooltip),
			m_valuePtr(valuePtr),
			m_minValue(minValue),
			m_maxValue(maxValue) {
		}

		bool RenderImGui() override {
			bool changed = ImGui::InputInt(m_name.c_str(), m_valuePtr);
			ShowTooltipIfHovered();

			// Clamp value to range
			if (*m_valuePtr < m_minValue) {
				*m_valuePtr = m_minValue;
				changed = true;
			}
			else if (*m_valuePtr > m_maxValue) {
				*m_valuePtr = m_maxValue;
				changed = true;
			}

			return changed;
		}

	private:
		int* m_valuePtr;
		int m_minValue;
		int m_maxValue;
	};

	/**
	 * Float property descriptor
	 */
	class FloatPropertyDescriptor : public PropertyDescriptor {
	public:
		FloatPropertyDescriptor(const std::string& name, float* valuePtr,
			float minValue = 0.0f, float maxValue = FLT_MAX,
			const std::string& tooltip = "",
			const char* format = "%.3f")
			: PropertyDescriptor(name, tooltip),
			m_valuePtr(valuePtr),
			m_minValue(minValue),
			m_maxValue(maxValue),
			m_format(format) {
		}

		bool RenderImGui() override {
			bool changed = ImGui::InputFloat(m_name.c_str(), m_valuePtr, 0.0f, 0.0f, m_format);
			ShowTooltipIfHovered();

			// Clamp value to range
			if (*m_valuePtr < m_minValue) {
				*m_valuePtr = m_minValue;
				changed = true;
			}
			else if (*m_valuePtr > m_maxValue) {
				*m_valuePtr = m_maxValue;
				changed = true;
			}

			return changed;
		}

	private:
		float* m_valuePtr;
		float m_minValue;
		float m_maxValue;
		const char* m_format;
	};

	/**
	 * Enum property descriptor
	 */
	template<typename EnumType>
	class EnumPropertyDescriptor : public PropertyDescriptor {
	public:
		EnumPropertyDescriptor(const std::string& name, EnumType* valuePtr,
			const std::vector<std::string>& options,
			const std::string& tooltip = "")
			: PropertyDescriptor(name, tooltip),
			m_valuePtr(valuePtr),
			m_options(options) {
		}

		bool RenderImGui() override {
			int currentIndex = static_cast<int>(*m_valuePtr);

			// Convert options to C-style strings for ImGui
			std::vector<const char*> optionsCStr;
			for (const auto& option : m_options) {
				optionsCStr.push_back(option.c_str());
			}

			bool changed = ImGui::Combo(m_name.c_str(), &currentIndex,
				optionsCStr.data(),
				static_cast<int>(optionsCStr.size()));

			ShowTooltipIfHovered();

			if (changed) {
				*m_valuePtr = static_cast<EnumType>(currentIndex);
			}

			return changed;
		}

	private:
		EnumType* m_valuePtr;
		std::vector<std::string> m_options;
	};

	/**
	 * String property descriptor
	 */
	class StringPropertyDescriptor : public PropertyDescriptor {
	public:
		StringPropertyDescriptor(const std::string& name, std::string* valuePtr,
			size_t maxLength = 128,
			const std::string& tooltip = "")
			: PropertyDescriptor(name, tooltip),
			m_valuePtr(valuePtr),
			m_maxLength(maxLength),
			m_buffer(new char[maxLength + 1]) {

			// Initialize buffer with current value
			strncpy(m_buffer.get(), m_valuePtr->c_str(), m_maxLength);
			m_buffer[m_maxLength] = '\0';
		}

		bool RenderImGui() override {
			bool changed = ImGui::InputText(m_name.c_str(), m_buffer.get(), m_maxLength + 1);
			ShowTooltipIfHovered();

			if (changed) {
				*m_valuePtr = m_buffer.get();
			}

			return changed;
		}

	private:
		std::string* m_valuePtr;
		size_t m_maxLength;
		std::unique_ptr<char[]> m_buffer;
	};

} // namespace Tool::UI