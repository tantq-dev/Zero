#include "UIMonsterPalate.h"
#include "core/EventSystem.h"
#include "EventKey.h"

void Tool::UI::UIMonsterPalate::ShowMonsterPalate(bool* p_open)
{
	if (p_open)
	{

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
				if (ImGui::MenuItem("Add 10000 items")) {

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
					const int item_max_idx_for_current_line = fmin((line_idx + 1) * column_count, MonsterItems.size());
					for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx)
					{
						MonsterItem* item_data = &MonsterItems[item_idx];
						ImGui::PushID((int)item_data->id);

						// Position item
						ImVec2 pos = ImVec2(start_pos.x + (item_idx % column_count) * LayoutItemStep.x, start_pos.y + line_idx * LayoutItemStep.y);
						ImGui::SetCursorScreenPos(pos);

						ImGui::SetNextItemSelectionUserData(item_idx);
						bool item_is_selected = selectedItem == item_data->id;
						bool item_is_visible = ImGui::IsRectVisible(LayoutItemSize);
						ImGui::Selectable("", item_is_selected, ImGuiSelectableFlags_None, LayoutItemSize);

						// Check for a simple click
						if (ImGui::IsItemClicked() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							if (selectedItem == item_data->id)
								selectedItem = -1; // Deselect if already selected
							else
								selectedItem = item_data->id;
							//todo: send event to map editor know which monster selected now
							Core::EventData eventData;
							eventData.data = *item_data;  // Send the entire monster item
							Core::EventSystem::getInstance().publish(EventKeys::MonsterSelectedFromPalate, eventData);
						}
						if (item_is_visible)
						{
							ImVec2 box_min(pos.x - 1, pos.y - 1);
							ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2); // Dubious
							draw_list->AddRectFilled(box_min, box_max, icon_bg_color); // Background color

							if (display_label)
							{
								ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
								char label[32];
								draw_list->AddText(ImVec2(box_min.x, box_max.y - ImGui::GetFontSize()), label_col, item_data->name.c_str());
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

void Tool::UI::UIMonsterPalate::AddItems(int count) {
	if (MonsterItems.size() == 0)
		NextItemId = 0;
	MonsterItems.reserve(MonsterItems.size() + count);
	for (int n = 0; n < count; n++, NextItemId++)
	{
		MonsterItems.push_back(MonsterItem(NextItemId, std::to_string(NextItemId % 20)));
	}
}

void Tool::UI::UIMonsterPalate::AddItems(std::vector < std::string> names) {
	if (MonsterItems.size() == 0)
		NextItemId = 0;
	MonsterItems.reserve(MonsterItems.size() + names.size());
	for (int n = 0; n < names.size(); n++, NextItemId++)
	{
		MonsterItems.push_back(MonsterItem(NextItemId, names[n]));
	}
}

void Tool::UI::UIMonsterPalate::UpdateLayoutSizes(float avail_width)
{
	// Layout: when not stretching: allow extending into right-most spacing.
	LayoutItemSpacing = (float)IconSpacing;
	if (StretchSpacing == false)
		avail_width += floorf(LayoutItemSpacing * 0.5f);

	// Layout: calculate number of icon per line and number of lines
	LayoutItemSize = ImVec2(floorf(IconSize), floorf(IconSize));
	LayoutColumnCount = fmax((int)(avail_width / (LayoutItemSize.x + LayoutItemSpacing)), 1);
	LayoutLineCount = (MonsterItems.size() + LayoutColumnCount - 1) / LayoutColumnCount;

	// Layout: when stretching: allocate remaining space to more spacing. Round before division, so item_spacing may be non-integer.
	if (StretchSpacing && LayoutColumnCount > 1)
		LayoutItemSpacing = floorf(avail_width - LayoutItemSize.x * LayoutColumnCount) / LayoutColumnCount;

	LayoutItemStep = ImVec2(LayoutItemSize.x + LayoutItemSpacing, LayoutItemSize.y + LayoutItemSpacing);
	LayoutSelectableSpacing = fmax(floorf(LayoutItemSpacing) - IconHitSpacing, 0.0f);
	LayoutOuterPadding = floorf(LayoutItemSpacing * 0.5f);
}