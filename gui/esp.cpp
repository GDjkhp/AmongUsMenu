#include "pch-il2cpp.h"
#include "esp.hpp"
#include "DirectX.h"
#include "utility.h"
#include "game.h"
#include "gui-helpers.hpp"

drawing_t* Esp::s_Instance = new drawing_t();
ImGuiWindow* CurrentWindow = nullptr;

static void RenderText(std::string_view text, const ImVec2& pos, const ImVec4& color, const bool outlined = true, const bool centered = true)
{
	if (text.empty()) return;
	ImVec2 ImScreen = pos;
	if (centered)
	{
		auto size = ImGui::CalcTextSize(text.data(), text.data() + text.length());
		ImScreen.x -= size.x * 0.5f;
		ImScreen.y -= size.y;
	}

	if (outlined)
	{
		CurrentWindow->DrawList->AddText(nullptr, 0.f,
			ImScreen + 0.5f * State.dpiScale,
			ImGui::GetColorU32(IM_COL32_BLACK), text.data(), text.data() + text.length());
	}

	CurrentWindow->DrawList->AddText(nullptr, 0.f, ImScreen, ImGui::GetColorU32(color), text.data(), text.data() + text.length());
}

static void RenderLine(const ImVec2& start, const ImVec2& end, const ImVec4& color, bool shadow = false) noexcept
{
	if (shadow)
	{
		CurrentWindow->DrawList->AddLine(
			start + 0.5f * State.dpiScale,
			end + 0.5f * State.dpiScale,
			ImGui::GetColorU32(color) & IM_COL32_A_MASK, 1.0f * State.dpiScale);
	}

	CurrentWindow->DrawList->AddLine(start, end, ImGui::GetColorU32(color), 1.0f * State.dpiScale);
}

static void RenderBox(const ImVec2& top, const ImVec2& bottom, const float height, const float width, const ImVec4& color, const bool wantsShadow = true)
{
	const ImVec2 points[] = {
		bottom, { bottom.x, ((float)(int)(bottom.y * 0.75f + top.y * 0.25f)) },
		{ bottom.x - 0.5f * State.dpiScale, bottom.y }, { ((float)(int)(bottom.x * 0.75f + top.x * 0.25f)), bottom.y },

		{ top.x + 0.5f * State.dpiScale, bottom.y }, { ((float)(int)(top.x * 0.75f + bottom.x * 0.25f)), bottom.y },
		{ top.x, bottom.y }, { top.x, ((float)(int)(bottom.y * 0.75f + top.y * 0.25f)) },

		{ bottom.x, top.y }, { bottom.x, ((float)(int)(top.y * 0.75f + bottom.y * 0.25f)) },
		{ bottom.x - 0.5f * State.dpiScale, top.y }, { ((float)(int)(bottom.x * 0.75f + top.x * 0.25f)), top.y },

		top, { ((float)(int)(top.x * 0.75f + bottom.x * 0.25f)), top.y },
		{ top.x, top.y + 0.5f * State.dpiScale }, { top.x, ((float)(int)(top.y * 0.75f + bottom.y * 0.25f)) }
	};

	if (wantsShadow) {
		const ImVec4& shadowColor = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(color) & IM_COL32_A_MASK);
		for (size_t i = 0; i < std::size(points); i += 2) {
			RenderLine(points[i] + 0.5f * State.dpiScale, points[i + 1] + 0.5f * State.dpiScale, shadowColor, false);
		}
	}
	for (size_t i = 0; i < std::size(points); i += 2) {
		RenderLine(points[i], points[i + 1], color, false);
	}
}

bool renderPlayerEsp = false;

void Esp::Render()
{
	CurrentWindow = ImGui::GetCurrentWindow();

	drawing_t& instance = Esp::GetDrawing();

	// Lock our mutex when we render (this will unlock when it goes out of scope)
	synchronized(instance.m_DrawingMutex) {
		// testing some stuffs
		if (renderPlayerEsp) {
			for (auto& it : instance.m_Players)
			{
				if (const auto& player = it.playerData.validate();
					player.has_value()						//Verify PlayerControl hasn't been destroyed (happens when disconnected)
					&& !player.is_Disconnected()		//Sanity check, shouldn't ever be true
					&& player.is_LocalPlayer()			//highlight yourself, you're ugly
					&& (!player.get_PlayerData()->fields.IsDead || State.ShowEsp_Ghosts)
					&& it.OnScreen)
				{
					if (State.ShowEsp_Box)
					{
						float width = GetScaleFromValue(35.0f);
						float height = GetScaleFromValue(120.0f);

						ImVec2 top{ it.Position.x + width, it.Position.y };
						ImVec2 bottom{ it.Position.x - width, it.Position.y - height };

						RenderBox(top, bottom, height, width, it.Color);
					}

					// TODO: show x and y
					if (State.ShowEsp_Distance)
					{
						const ImVec2 position{ it.Position.x, it.Position.y + 15.0f * State.dpiScale };

						Vector2 mouse = {
							ImGui::GetMousePos().x, ImGui::GetMousePos().y
						};

						std::string lel = std::to_string(ScreenToWorld(mouse).x) + ", " + std::to_string(ScreenToWorld(mouse).y);

						std::string lol = std::to_string(it.Position.x) + ", " + std::to_string(it.Position.y);
						char* player = lol.data();

						RenderText(player, position, it.Color);
					}
				}
			}
		}
		// track player codes
		for (auto& it : instance.m_Players)
		{
			if (const auto& player = it.playerData.validate();
				player.has_value()						//Verify PlayerControl hasn't been destroyed (happens when disconnected)
				&& !player.is_Disconnected()		//Sanity check, shouldn't ever be true
				&& !player.is_LocalPlayer()			//Don't highlight yourself, you're ugly
				&& (!player.get_PlayerData()->fields.IsDead || State.ShowEsp_Ghosts)
				&& it.OnScreen)
			{
				/////////////////////////////////
				//// Box ////////////////////////
				/////////////////////////////////
				if (State.ShowEsp_Box)
				{
					float width = GetScaleFromValue(35.0f);
					float height = GetScaleFromValue(120.0f);

					ImVec2 top{ it.Position.x + width, it.Position.y };
					ImVec2 bottom{ it.Position.x - width, it.Position.y - height };

					RenderBox(top, bottom, height, width, it.Color);
				}
				/////////////////////////////////
				//// Distance ///////////////////
				/////////////////////////////////

				float killTimer = -1.f;
				if (auto role = player.get_PlayerData()->fields.Role;
					role->fields.CanUseKillButton && !player.get_PlayerData()->fields.IsDead) {
					killTimer = player.get_PlayerControl()->fields.killTimer;
				}

				if (State.ShowEsp_Distance || killTimer >= 0.f)
				{
					// logic and calculation
					ImVec2 position = { it.Position.x, it.Position.y + 15.0f * State.dpiScale };
					ImVec2 position2 = { it.Position.x, it.Position.y + 30.0f * State.dpiScale };

					// infamous trash codes
					float minX = 40.0f, minY = 0.0f, 
						maxX = DirectX::GetWindowSize().x - 46.0f, // 1320
						maxY = DirectX::GetWindowSize().y - 38.0f; // 730

					if (it.Position.x < minX) {
						if (it.Position.y < minY) {
							position = { minX, minY + 15.0f * State.dpiScale };
							position2 = { minX, minY + 30.0f * State.dpiScale };
						}
						else if (it.Position.y > maxY) {
							position = { minX, maxY + 15.0f * State.dpiScale };
							position2 = { minX, maxY + 30.0f * State.dpiScale };
						}
						else {
							position = { minX, it.Position.y + 15.0f * State.dpiScale };
							position2 = { minX, it.Position.y + 30.0f * State.dpiScale };
						}

					}
					else if (it.Position.x > maxX) {
						if (it.Position.y > maxY) {
							position = { maxX, maxY + 15.0f * State.dpiScale };
							position2 = { maxX, maxY + 30.0f * State.dpiScale };
						}
						else if (it.Position.y < minY) {
							position = { maxX, minY + 15.0f * State.dpiScale };
							position2 = { maxX, minY + 30.0f * State.dpiScale };
						}
						else {
							position = { maxX, it.Position.y + 15.0f * State.dpiScale };
							position2 = { maxX, it.Position.y + 30.0f * State.dpiScale };
						}
					}
					else {
						if (it.Position.y < minY) {
							position = { it.Position.x, minY + 15.0f * State.dpiScale };
							position2 = { it.Position.x, minY + 30.0f * State.dpiScale };
						}
						else if (it.Position.y > maxY) {
							position = { it.Position.x, maxY + 15.0f * State.dpiScale };
							position2 = { it.Position.x, maxY + 30.0f * State.dpiScale };
						}
						else {
							position = { it.Position.x, it.Position.y + 15.0f * State.dpiScale };
							position2 = { it.Position.x, it.Position.y + 30.0f * State.dpiScale };
						}
					}

					// strings
					char distance[32];
					sprintf_s(distance, "[%.0fm]", it.Distance);
					
					std::string lol = it.Name;
					char* player = lol.data();

					//std::string lol2 = std::to_string(it.Position.x) + ", " + std::to_string(it.Position.y);
					//char* pl = lol2.data();

					// kill cd code
					std::string label;
					if (State.ShowEsp_Distance) {
						char distance[32] = { 0 };
						sprintf_s(distance, "[%.0fm]", it.Distance);
						label = distance;
					}
					if (killTimer >= 0.f) {
						char distance[32] = { 0 };
						sprintf_s(distance, "[Kill CD:%.1fs]", killTimer);
						label += distance;
					}
					char* killcd = label.data();

					// render info
					RenderText(player, position, it.ColorId);
					RenderText(killcd, position2, it.Color);
				}
				/////////////////////////////////
				//// Tracers ////////////////////
				/////////////////////////////////
				if (State.ShowEsp_Tracers)
				{
					RenderLine(instance.LocalPosition, it.Position, it.Color, true);
				}
			}
		}
	}
}
