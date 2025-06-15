#include "UIMonsterPalette.h"
#include "core/EventSystem.h"
#include "EventKey.h"
#include "MonsterModel.h"
#include "resources/ResourcesManager.h"

Tool::UI::UIMonsterPalette::UIMonsterPalette()
{
	std::vector<std::string> keys;

	for (const auto& pair : MonsterConfig) {
		AddItem(pair.first, pair.second);
	}

	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterUpdated, [this](const Core::EventData& eventData) {
		const auto& monsterType = eventData.get<MonsterTypeDefinition>();
		m_monsterTypeRegistry.UpdateDefaultProperties(monsterType.item.name, monsterType.defaultProperties);
		m_cacheNeedsUpdate = true; // Ensure cache is updated after selection
		});

	// Initialize popup state
	m_showAddMonsterPopup = false;
	memset(m_newMonsterName, 0, sizeof(m_newMonsterName));
}

void Tool::UI::UIMonsterPalette::ShowMonsterPalette(bool* p_open)
{
	if (p_open)
	{
		if (m_cacheNeedsUpdate)
		{
			UpdateCache();
		}

		bool window_contents_visible = ImGui::Begin("Monster Palate", p_open, ImGuiWindowFlags_MenuBar);
		if (!window_contents_visible)
		{
			ImGui::End();
			return;
		}

		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Add Monster")) {
					m_showAddMonsterPopup = true;
					// Clear the input field when opening the popup
					memset(m_newMonsterName, 0, sizeof(m_newMonsterName));
				}

				if (ImGui::MenuItem("Clear items")) {

				}

				ImGui::Separator();
				if (ImGui::MenuItem("Close", NULL, false, p_open != NULL)) {
					*p_open = false;

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Handle Add Monster Popup
		ShowAddMonsterPopup();

		ImGui::SetNextWindowContentSize(ImVec2(0.0f, LayoutOuterPadding + LayoutLineCount * (LayoutItemSize.x + LayoutItemSpacing)));
		if (ImGui::BeginChild("Assets", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove))
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			const float avail_width = ImGui::GetContentRegionAvail().x;
			UpdateLayoutSizes(avail_width);

			// Calculate and store start position.
			ImVec2 start_pos = ImGui::GetCursorScreenPos();
			start_pos = ImVec2(start_pos.x + LayoutOuterPadding, start_pos.y + LayoutOuterPadding);
			ImGui::SetCursorScreenPos(start_pos);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));

			// Rendering parameters
			const ImU32 icon_type_overlay_colors[3] = { 0, IM_COL32(200, 70, 70, 255), IM_COL32(70, 170, 70, 255) };
			const ImU32 icon_bg_color = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
			const ImVec2 icon_type_overlay_size = ImVec2(4.0f, 4.0f);
			const bool display_label = (LayoutItemSize.x >= ImGui::CalcTextSize("9999").x);

			const int column_count = LayoutColumnCount;

			ImGuiListClipper clipper;
			clipper.Begin(LayoutLineCount, LayoutItemStep.y);

			while (clipper.Step())
			{
				for (int line_idx = clipper.DisplayStart; line_idx < clipper.DisplayEnd; line_idx++)
				{
					const int item_min_idx_for_current_line = line_idx * column_count;
					const int item_max_idx_for_current_line = fmin((line_idx + 1) * column_count, m_monsterTypeRegistry.GetAllTypes().size());
					for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx)
					{
						MonsterTypeDefinition* item_data = m_cachedMonsterTypes[item_idx];
						ImGui::PushID((int)item_data->item.id);

						// Position item
						ImVec2 pos = ImVec2(start_pos.x + (item_idx % column_count) * LayoutItemStep.x, start_pos.y + line_idx * LayoutItemStep.y);
						ImGui::SetCursorScreenPos(pos);

						ImGui::SetNextItemSelectionUserData(item_idx);
						bool item_is_selected = selectedItem == item_data->item.id;
						bool item_is_visible = ImGui::IsRectVisible(LayoutItemSize);
						ImGui::Selectable("", item_is_selected, ImGuiSelectableFlags_None, LayoutItemSize);

						// Check for a simple click
						if (ImGui::IsItemClicked() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							if (selectedItem == item_data->item.id)
								selectedItem = -1; // Deselect if already selected
							else
								selectedItem = item_data->item.id;
							//todo: send event to map editor know which monster selected now
							Core::EventData eventData;
							eventData.data = *item_data;  // Send the entire monster item
							Core::EventSystem::getInstance().publish(EventKeys::MonsterSelectedFromPalette, eventData);
						}

						if (item_is_visible)
						{
							ImVec2 box_min(pos.x - 1, pos.y - 1);
							ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2);

							// Draw background color
							draw_list->AddRectFilled(box_min, box_max, icon_bg_color);

							// Get texture for the monster from ResourcesManager
							ImTextureID texture_id = (ImTextureID)(intptr_t)ResourcesManager::GetInstance().GetTexture(item_data->item.name);

							// Calculate image area (slightly smaller than box to create padding)
							const float padding = 2.0f;
							ImVec2 image_min(box_min.x + padding, box_min.y + padding);
							ImVec2 image_max(box_max.x - padding,
								display_label ? box_max.y - ImGui::GetFontSize() - padding : box_max.y - padding);

							// Draw the monster texture if available
							if (texture_id)
							{
								draw_list->AddImage(texture_id, image_min, image_max);
							}
							else
							{
								// If no texture, draw a placeholder with the monster name
								ImU32 placeholder_col = IM_COL32(100, 100, 100, 255);
								draw_list->AddRectFilled(image_min, image_max, placeholder_col);

								// Center the first letter of monster name as a placeholder
								if (item_data->item.name.length() > 0) {
									char letter[2] = { item_data->item.name[0], '\0' };
									ImVec2 text_size = ImGui::CalcTextSize(letter);
									ImVec2 text_pos(
										(image_min.x + image_max.x - text_size.x) * 0.5f,
										(image_min.y + image_max.y - text_size.y) * 0.5f
									);
									draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), letter);
								}
							}

							if (display_label)
							{
								ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
								draw_list->AddText(ImVec2(box_min.x, box_max.y - ImGui::GetFontSize()), label_col, item_data->item.name.c_str());
							}
						}

						ImGui::PopID();
					}
				}
			}

			ImGui::PopStyleVar();

			ImGui::EndChild();

		}

		ImGui::End();

	}
}

void Tool::UI::UIMonsterPalette::ShowAddMonsterPopup()
{
	if (m_showAddMonsterPopup)
	{
		ImGui::OpenPopup("Add New Monster");
	}

	// Center the popup
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Appearing);

	if (ImGui::BeginPopupModal("Add New Monster", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter monster name:");
		ImGui::Separator();

		// Input field for monster name
		ImGui::SetNextItemWidth(250.0f);
		bool enterPressed = ImGui::InputText("##MonsterName", m_newMonsterName, sizeof(m_newMonsterName),
			ImGuiInputTextFlags_EnterReturnsTrue);

		ImGui::Spacing();

		// Button layout
		float buttonWidth = 80.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float totalWidth = buttonWidth * 2 + spacing;
		float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

		ImGui::SetCursorPosX(startX);

		// Add button
		if (ImGui::Button("Add", ImVec2(buttonWidth, 0)) || enterPressed)
		{
			std::string monsterName = std::string(m_newMonsterName);

			// Check if name is not empty and doesn't already exist
			if (!monsterName.empty() && m_monsterTypeRegistry.GetAllTypes().find(monsterName) == m_monsterTypeRegistry.GetAllTypes().end())
			{
				// Add new monster with default "Slug" texture
				AddItem(monsterName, "assets//Slug.bmp");
				m_cacheNeedsUpdate = true;

				// Close popup and reset
				m_showAddMonsterPopup = false;
				memset(m_newMonsterName, 0, sizeof(m_newMonsterName));
				ImGui::CloseCurrentPopup();
			}
			else if (monsterName.empty())
			{
				// Show error for empty name (you could add a status message here)
			}
			else
			{
				// Show error for duplicate name (you could add a status message here)
			}
		}

		ImGui::SameLine();

		// Cancel button
		if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
		{
			m_showAddMonsterPopup = false;
			memset(m_newMonsterName, 0, sizeof(m_newMonsterName));
			ImGui::CloseCurrentPopup();
		}

		// Handle ESC key to close popup
		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			m_showAddMonsterPopup = false;
			memset(m_newMonsterName, 0, sizeof(m_newMonsterName));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void Tool::UI::UIMonsterPalette::AddItems(int count) {
	// This method is now unused since we're adding one monster at a time via popup
}

void Tool::UI::UIMonsterPalette::AddItem(std::string name, const char* path)
{
	m_monsterTypeRegistry.RegisterMonsterType(name, path);
	UpdateCache();
}

void Tool::UI::UIMonsterPalette::UpdateLayoutSizes(float avail_width)
{
	// Layout: when not stretching: allow extending into right-most spacing.
	LayoutItemSpacing = (float)IconSpacing;
	if (StretchSpacing == false)
		avail_width += floorf(LayoutItemSpacing * 0.5f);

	// Layout: calculate number of icon per line and number of lines
	LayoutItemSize = ImVec2(floorf(IconSize), floorf(IconSize));
	LayoutColumnCount = fmax((int)(avail_width / (LayoutItemSize.x + LayoutItemSpacing)), 1);
	LayoutLineCount = (m_cachedMonsterTypes.size() + LayoutColumnCount - 1) / LayoutColumnCount;

	// Layout: when stretching: allocate remaining space to more spacing. Round before division, so item_spacing may be non-integer.
	if (StretchSpacing && LayoutColumnCount > 1)
		LayoutItemSpacing = floorf(avail_width - LayoutItemSize.x * LayoutColumnCount) / LayoutColumnCount;

	LayoutItemStep = ImVec2(LayoutItemSize.x + LayoutItemSpacing, LayoutItemSize.y + LayoutItemSpacing);
	LayoutSelectableSpacing = fmax(floorf(LayoutItemSpacing) - IconHitSpacing, 0.0f);
	LayoutOuterPadding = floorf(LayoutItemSpacing * 0.5f);
}

void Tool::UI::UIMonsterPalette::UpdateCache()
{
	m_cachedMonsterTypes.clear();
	m_cachedMonsterTypes.reserve(m_monsterTypeRegistry.GetAllTypes().size());
	for (const auto& pair : m_monsterTypeRegistry.GetAllTypes()) {
		m_cachedMonsterTypes.push_back(pair.second.get());
	}
	m_cacheNeedsUpdate = false;
}