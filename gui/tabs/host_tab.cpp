#include "pch-il2cpp.h"
#include "host_tab.h"
#include "utility.h"
#include "game.h"
#include "state.hpp"
#include "gui-helpers.hpp"

namespace HostTab {
	static void SetRoleAmount(RoleTypes__Enum type, int amount) {
		auto&& options = GameOptions().GetRoleOptions();
		auto maxCount = options.GetNumPerGame(type);
		if (amount > maxCount)
			options.SetRoleRate(type, amount, 100);
		else if (amount > 0)
			options.SetRoleRate(type, maxCount, 100);
	}

	void Render() {
		if (ImGui::BeginTabItem("Host")) {
			GameOptions options;
			if (IsHost() && IsInLobby()) {
				ImGui::Text("Select Roles:");
				ImGui::BeginChild("host#list", ImVec2(200, 0) * State.dpiScale, true);
				bool shouldEndListBox = ImGui::ListBoxHeader("Choose Roles", ImVec2(200, 290) * State.dpiScale);
				auto allPlayers = GetAllPlayerData();
				auto playerAmount = allPlayers.size();
				auto maxImposterAmount = GetMaxImposterAmount((int)playerAmount);
				for (size_t index = 0; index < playerAmount; index++) {
					auto playerData = allPlayers[index];
					PlayerControl* playerCtrl = GetPlayerControlById(playerData->fields.PlayerId);

					State.assignedRolesPlayer[index] = playerCtrl;
					if (State.assignedRolesPlayer[index] == nullptr)
						continue;

					app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(playerData);
					if (outfit == NULL) continue;
					const std::string& playerName = convert_from_string(GameData_PlayerOutfit_get_PlayerName(outfit, nullptr));
					if (CustomListBoxInt(playerName.c_str(), reinterpret_cast<int*>(&State.assignedRoles[index]), ROLE_NAMES, 80 * State.dpiScale, AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId))))
					{
						State.engineers_amount = (int)GetRoleCount(RoleType::Engineer);
						State.scientists_amount = (int)GetRoleCount(RoleType::Scientist);
						State.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
						State.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
						State.crewmates_amount = (int)GetRoleCount(RoleType::Crewmate);

						if (State.impostors_amount + State.shapeshifters_amount > maxImposterAmount)
						{
							if(State.assignedRoles[index] == RoleType::Shapeshifter)
								State.assignedRoles[index] = RoleType::Random; //Set to random to avoid bugs.
							else if(State.assignedRoles[index] == RoleType::Impostor)
								State.assignedRoles[index] = RoleType::Random;
						}

						if (State.assignedRoles[index] == RoleType::Engineer || State.assignedRoles[index] == RoleType::Scientist || State.assignedRoles[index] == RoleType::Crewmate) {			
							if (State.engineers_amount + State.scientists_amount + State.crewmates_amount >= (int)playerAmount)
								State.assignedRoles[index] = RoleType::Random;
						} //Some may set all players to non imps. This hangs the game on beginning. Leave space to Random so we have imps.

						if (options.GetGameMode() == GameModes__Enum::HideNSeek)
						{
							if (State.assignedRoles[index] == RoleType::Shapeshifter)
								State.assignedRoles[index] = RoleType::Random;
							else if (State.assignedRoles[index] == RoleType::Scientist)
								State.assignedRoles[index] = RoleType::Engineer;
							else if (State.assignedRoles[index] == RoleType::Crewmate)
								State.assignedRoles[index] = RoleType::Engineer;
						} //Assign other roles in hidenseek causes game bug.
						//These are organized. Do not change the order unless you find it necessary.

						if (!IsInGame()) {
							SetRoleAmount(RoleTypes__Enum::Engineer, State.engineers_amount);
							SetRoleAmount(RoleTypes__Enum::Scientist, State.scientists_amount);
							SetRoleAmount(RoleTypes__Enum::Shapeshifter, State.shapeshifters_amount);
							if(options.GetNumImpostors() <= State.impostors_amount + State.shapeshifters_amount)
								options.SetInt(app::Int32OptionNames__Enum::NumImpostors, State.impostors_amount + State.shapeshifters_amount);
						}
					}
				}
				if (shouldEndListBox)
					ImGui::ListBoxFooter();
				ImGui::EndChild();
				ImGui::SameLine();
			}

			ImGui::BeginChild("host#actions", ImVec2(300, 0) * State.dpiScale, true);

			// AU v2022.8.24 has been able to change maps in lobby.
			State.mapHostChoice = options.GetByte(app::ByteOptionNames__Enum::MapId);
			if (State.mapHostChoice > 3)
				State.mapHostChoice--;
			State.mapHostChoice = std::clamp(State.mapHostChoice, 0, (int)MAP_NAMES.size() - 1);
			if (IsHost() && IsInLobby() && CustomListBoxInt("Map", &State.mapHostChoice, MAP_NAMES, 75 * State.dpiScale)) {
				if (!IsInGame()) {
					// disable flip
					/*if (State.mapHostChoice == 3) {
						options.SetByte(app::ByteOptionNames__Enum::MapId, 0);
						State.FlipSkeld = true;
					}
					else {
						options.SetByte(app::ByteOptionNames__Enum::MapId, State.mapHostChoice);
						State.FlipSkeld = false;
					}*/
					auto id = State.mapHostChoice;
					if (id >= 3) id++;
					options.SetByte(app::ByteOptionNames__Enum::MapId, id);
				}
			}
			//ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			if (IsHost() && IsInLobby() && ImGui::Button("Force Start of Game"))
			{
				app::InnerNetClient_SendStartGame((InnerNetClient*)(*Game::pAmongUsClient), NULL);
			}
			//ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			if (IsHost() && IsInLobby() && ImGui::Checkbox("Modify impostor count", &State.impostor_mod))
				State.Save();
			/*if (State.impostor_mod &&
				CustomListBoxInt("Impostor count", &State.impostors_amount_mod, IMPOSTOR_AMOUNTS, 75 * State.dpiScale)) {
				if (State.impostors_amount_mod < 0 || State.impostors_amount_mod > 15) State.impostors_amount_mod = 0;
			}*/
			State.impostors_amount_mod = std::clamp(State.impostors_amount_mod, 0, int(Game::MAX_PLAYERS));
			if (IsHost() && IsInLobby() && State.impostor_mod && ImGui::InputInt("Count", &State.impostors_amount_mod))
				State.Save();
			if (ImGui::Checkbox("Disable Meetings", &State.DisableMeetings))
				State.Save();
			if (ImGui::Checkbox("Disable Sabotages", &State.DisableSabotages))
				State.Save();
			if (IsInMultiplayerGame() || IsInLobby()) { //lobby isn't possible in freeplay
				if (IsInGame()) {
					CustomListBoxInt("Reason", &State.SelectedGameEndReasonId, GAMEENDREASON, 120.0f * State.dpiScale);
					if (ImGui::Button("End Game")) {
						State.rpcQueue.push(new RpcEndGame(GameOverReason__Enum(std::clamp(State.SelectedGameEndReasonId, 0, 8))));
					}
				}
			}
			// hacked game settings
			if (IsHost() && IsInLobby())
				ImGui::Checkbox("Unlock settings", &State.ToggleHackSettings);
			if (IsHost() && IsInLobby() && State.ToggleHackSettings) {
				// load settings?
				State.NumImpostors = options.GetInt(app::Int32OptionNames__Enum::NumImpostors);
				State.KillDistanceHost = options.GetInt(app::Int32OptionNames__Enum::KillDistance);
				State.CrewVents = options.GetInt(app::Int32OptionNames__Enum::CrewmateVentUses);

				State.CommonTasks = options.GetInt(app::Int32OptionNames__Enum::NumCommonTasks);
				State.ShortTasks = options.GetInt(app::Int32OptionNames__Enum::NumShortTasks);
				State.LongTasks = options.GetInt(app::Int32OptionNames__Enum::NumLongTasks);

				State.KillCooldown = options.GetFloat(app::FloatOptionNames__Enum::KillCooldown);
				State.EscapeTime = options.GetFloat(app::FloatOptionNames__Enum::EscapeTime);
				State.FinalEscapeTime = options.GetFloat(app::FloatOptionNames__Enum::FinalEscapeTime);
				State.EngineerVentTime = options.GetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime);

				// useless, remove this from state later
				/*if (ImGui::InputInt("NumImpostors", &State.NumImpostors))
					options.SetInt(app::Int32OptionNames__Enum::NumImpostors, State.NumImpostors);
				if (ImGui::InputInt("KillDistance", &State.KillDistanceHost))
					options.SetInt(app::Int32OptionNames__Enum::KillDistance, State.KillDistanceHost);
				if (options.GetGameMode() == GameModes__Enum::Normal &&
					ImGui::InputFloat("EngineerInVentMaxTime", &State.EngineerVentTime))
					options.SetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime, State.EngineerVentTime);*/
				
				bool commonTasksChanged = ImGui::InputInt("CommonTasks", &State.CommonTasks);
				bool shortTasksChanged = ImGui::InputInt("ShortTasks", &State.ShortTasks);
				bool longTasksChanged = ImGui::InputInt("LongTasks", &State.LongTasks);

				if (commonTasksChanged || shortTasksChanged || longTasksChanged) {
					// Ensure values are within individual limits first
					State.CommonTasks = std::clamp(State.CommonTasks, 0, 255);
					State.ShortTasks = std::clamp(State.ShortTasks, 0, 255);
					State.LongTasks = std::clamp(State.LongTasks, 0, 255);

					int totalTasks = State.CommonTasks + State.ShortTasks + State.LongTasks;
					if (totalTasks > 255) {
						int excess = totalTasks - 255;

						// Adjust the most recently changed value to fit the total within 255
						if (commonTasksChanged) {
							State.CommonTasks = max(0, State.CommonTasks - excess);
						}
						else if (shortTasksChanged) {
							State.ShortTasks = max(0, State.ShortTasks - excess);
						}
						else if (longTasksChanged) {
							State.LongTasks = max(0, State.LongTasks - excess);
						}

					}

					// Update values in options only if within the overall range
					if (commonTasksChanged) {
						options.SetInt(app::Int32OptionNames__Enum::NumCommonTasks, State.CommonTasks);
					}
					if (shortTasksChanged) {
						options.SetInt(app::Int32OptionNames__Enum::NumShortTasks, State.ShortTasks);
					}
					if (longTasksChanged) {
						options.SetInt(app::Int32OptionNames__Enum::NumLongTasks, State.LongTasks);
					}
				}


				ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);
				ImGui::Separator();
				ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);

				if (options.GetGameMode() == GameModes__Enum::HideNSeek &&
					ImGui::InputInt("CrewmateVentUses", &State.CrewVents))
					options.SetInt(app::Int32OptionNames__Enum::CrewmateVentUses, State.CrewVents);

				if (ImGui::InputFloat("KillCooldown", &State.KillCooldown))
					options.SetFloat(app::FloatOptionNames__Enum::KillCooldown, State.KillCooldown);
				if (ImGui::Button("ZERO KILL COOLDOWN"))
					options.SetFloat(app::FloatOptionNames__Enum::KillCooldown, 1.4E-45f); // force cooldown > 0 as ur unable to kill otherwise

				ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);
				ImGui::Separator();
				ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);

				if (options.GetGameMode() == GameModes__Enum::HideNSeek) {
					if (ImGui::InputFloat("EscapeTime", &State.EscapeTime))
						options.SetFloat(app::FloatOptionNames__Enum::EscapeTime, State.EscapeTime);
					if (ImGui::InputFloat("FinalEscapeTime", &State.FinalEscapeTime))
						options.SetFloat(app::FloatOptionNames__Enum::FinalEscapeTime, State.FinalEscapeTime);
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
	}
	const ptrdiff_t GetRoleCount(RoleType role)
	{
		return std::count_if(State.assignedRoles.cbegin(), State.assignedRoles.cend(), [role](RoleType i) {return i == role; });
	}
}