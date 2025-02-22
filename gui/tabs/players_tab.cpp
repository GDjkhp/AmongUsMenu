#include "pch-il2cpp.h"
#include "players_tab.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"

namespace PlayersTab {

	int framesPassed = -1;
	Vector2 previousPlayerPosition;

	void Render() {
		if (IsInGame() || IsInLobby()) {
			if (ImGui::BeginTabItem("Players")) {
				ImGui::BeginChild("players#list", ImVec2(200, 0) * State.dpiScale, true);
				auto selectedPlayer = State.selectedPlayer.validate();
				bool shouldEndListBox = ImGui::ListBoxHeader("###players#list", ImVec2(200, 290) * State.dpiScale);
				auto localData = GetPlayerData(*Game::pLocalPlayer);
				for (auto playerData : GetAllPlayerData()) {
					const auto& player = PlayerSelection(playerData).validate();
					if (!player.has_value())
						continue;
					if (player.is_Disconnected())
						continue;

					app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(playerData);
					if (outfit == NULL) continue;
					std::string playerName = convert_from_string(GameData_PlayerOutfit_get_PlayerName(outfit, nullptr));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0) * State.dpiScale);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0) * State.dpiScale);
					if (ImGui::Selectable(std::string("##" + playerName).c_str(), selectedPlayer.equals(player))) {
						State.selectedPlayer = player;
						selectedPlayer = player;
					}
					ImGui::SameLine();
					ImGui::ColorButton(std::string("##" + playerName + "_ColorButton").c_str(), AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId)), ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip);
					ImGui::SameLine();
					ImGui::PopStyleVar(2);
					ImGui::Dummy(ImVec2(0, 0) * State.dpiScale);
					ImGui::SameLine();

					ImVec4 nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->White);
					if (State.RevealRoles)
					{
						std::string roleName = GetRoleName(playerData->fields.Role);
						playerName = playerName + " (" + roleName + ")";
						nameColor = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
					}
					else if(PlayerIsImpostor(localData) && PlayerIsImpostor(playerData))
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->ImpostorRed);
					else if (player.is_LocalPlayer() || std::count(State.aumUsers.begin(), State.aumUsers.end(), playerData->fields.PlayerId))
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->Orange);

					if (playerData->fields.IsDead)
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->DisabledGrey);

					ImGui::TextColored(nameColor, playerName.c_str());
				}
				if (shouldEndListBox)
					ImGui::ListBoxFooter();
				ImGui::EndChild();
				ImGui::SameLine();
				ImGui::BeginChild("players#actions", ImVec2(200, 0) * State.dpiScale, true);
				if (selectedPlayer.has_value()) //Upon first startup no player is selected.  Also rare case where the playerdata is deleted before the next gui cycle
				{
					bool aum = selectedPlayer.is_LocalPlayer() || std::count(State.aumUsers.begin(), State.aumUsers.end(), selectedPlayer.get_PlayerData()->fields.PlayerId);
					ImGui::TextColored(aum ? ImVec4(0, 1.0f, 0, 1.0f) : ImVec4(1.0f, 0, 0, 1.0f), 
						"Is using AUM: %s", aum ? "Yes" : "No");
				}
				if (IsInMultiplayerGame() && IsInGame()) {
					float taskPercentage = (float)(*Game::pGameData)->fields.CompletedTasks / (float)(*Game::pGameData)->fields.TotalTasks;
					ImGui::TextColored(ImVec4(1.0f - taskPercentage, 1.0f, 1.0f - taskPercentage, 1.0f), "%.1f%% Total Tasks Completed", taskPercentage * 100);
				}
				//GameOptions options;
				if (IsInGame() /* && options.GetGameMode() != GameModes__Enum::HideNSeek */ && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead) { //Player selection doesn't matter
					if (ImGui::Button("Call Meeting")) {
						State.rpcQueue.push(new RpcReportPlayer({}));
					}
				}
				if (State.activeImpersonation)
				{
					if (ImGui::Button("Reset Impersonation"))
					{
						std::queue<RPCInterface*> *queue = nullptr;

						if (IsInGame())
							queue = &State.rpcQueue;
						else if (IsInLobby())
							queue = &State.lobbyRpcQueue;
						if (queue != nullptr) {
							queue->push(new RpcSetColor(State.originalColor));
							queue->push(new RpcSetPet(State.originalPet));
							queue->push(new RpcSetSkin(State.originalSkin));
							queue->push(new RpcSetHat(State.originalHat));
							queue->push(new RpcSetVisor(State.originalVisor));
							queue->push(new RpcSetNamePlate(State.originalNamePlate));
							queue->push(new RpcSetName(State.originalName));
							State.activeImpersonation = false;
						}
						if (IsInLobby())
							ResetOriginalAppearance();
					}
				}
				if (selectedPlayer.has_value())
				{
					if (IsInGame() && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && selectedPlayer.get_PlayerData()->fields.IsDead) {
						ImGui::NewLine();
						if (ImGui::Button("Report Body")) {
							State.rpcQueue.push(new RpcReportPlayer(selectedPlayer));
						}
					}
					ImGui::NewLine();

					if (IsInGame() && !selectedPlayer.is_Disconnected() && !selectedPlayer.is_LocalPlayer())
					{
						if (State.playerToFollow.equals(selectedPlayer)) {
							if (ImGui::Button("Stop Spectating")) {
								State.playerToFollow.reset();
							}
						} else {
							if (ImGui::Button("Spectate")) {
								State.FreeCam = false;
								State.playerToFollow = selectedPlayer;
							}
						}
					}

					if (selectedPlayer.is_LocalPlayer() && State.originalName != "-") {
						if (ImGui::Button("Reset Name")) {
							if (IsInGame())
								State.rpcQueue.push(new RpcSetName(State.originalName));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcSetName(State.originalName));
						}
					}
					else if(!selectedPlayer.is_LocalPlayer()) {
						if ((IsInMultiplayerGame() || IsInLobby()) && ImGui::Button("Steal Name")) {
							ImpersonateName(selectedPlayer.get_PlayerData());
						}
					}
					if ((IsInGame() || IsInLobby())) {
						if (!selectedPlayer.is_LocalPlayer()) {
							app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
							if (outfit != NULL) {
								if (ImGui::Button("Impersonate")) {

									auto petId = outfit->fields.PetId;
									auto skinId = outfit->fields.SkinId;
									auto hatId = outfit->fields.HatId;
									auto visorId = outfit->fields.VisorId;
									auto colorId = outfit->fields.ColorId;
									auto namePlateId = outfit->fields.NamePlateId;
									std::queue<RPCInterface*>* queue = nullptr;

									if (IsInGame())
										queue = &State.rpcQueue;
									else if (IsInLobby()) {
										queue = &State.lobbyRpcQueue;
										if (State.originalColor == Game::NoColorId) {
											SaveOriginalAppearance();
										}
									}

									if (queue != nullptr) {
										if (IsHost())
											queue->push(new RpcSetColor(colorId, true));
										else
											queue->push(new RpcSetColor(GetRandomColorId()));
										queue->push(new RpcSetPet(petId));
										queue->push(new RpcSetSkin(skinId));
										queue->push(new RpcSetVisor(visorId));
										queue->push(new RpcSetHat(hatId));
										queue->push(new RpcSetNamePlate(namePlateId));
										ImpersonateName(selectedPlayer.get_PlayerData());
										State.activeImpersonation = true;
									}
								}
							}
						}
					}

					if (ImGui::Button("Kill"))
					{
						if (IsInGame()) {
							State.rpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), selectedPlayer.get_PlayerControl()));
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), selectedPlayer.get_PlayerControl()));
						}
					}

					if (IsInGame() && PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))
						&& !selectedPlayer.get_PlayerData()->fields.IsDead
						&& !selectedPlayer.get_PlayerControl()->fields.inVent
						&& !selectedPlayer.get_PlayerControl()->fields.inMovingPlat
						&& !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && ((*Game::pLocalPlayer)->fields.killTimer <= 0.0f)
						&& selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId <= -1)
					{
						if (ImGui::Button("Kill Player"))
						{
							previousPlayerPosition = GetTrueAdjustedPosition(*Game::pLocalPlayer);
							State.rpcQueue.push(new CmdCheckMurder(selectedPlayer));
							framesPassed = 40;
						}
					}

					if (IsHost() && ImGui::Button("Kick")) {
						app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), selectedPlayer.get_PlayerControl()->fields._.OwnerId, false, NULL);
					}
					if (ImGui::Button("Votekick")) {
						if (IsInGame() || IsInLobby()) {
							for (auto player : GetAllPlayerControl())
								State.rpcQueue.push(new RpcVoteKick(selectedPlayer.get_PlayerControl()));
						}
					}
					ImGui::SameLine();
					if (IsHost() && ImGui::Button("Ban")) {
						app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), selectedPlayer.get_PlayerControl()->fields._.OwnerId, true, NULL);
					}

					if (framesPassed == 0)
					{
						State.rpcQueue.push(new RpcSnapTo(previousPlayerPosition));
						framesPassed--;
					}
					else framesPassed--;

					app::RoleBehaviour* playerRole = localData->fields.Role;
					app::RoleTypes__Enum role = playerRole != nullptr ? playerRole->fields.Role : app::RoleTypes__Enum::Crewmate;

					if (ImGui::Button("Shift"))
					{
						if (IsInGame())
							State.rpcQueue.push(new RpcShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
					}
					else if (role == RoleTypes__Enum::Shapeshifter) {
						app::ShapeshifterRole* shapeshifterRole = (app::ShapeshifterRole*)playerRole;
						if (shapeshifterRole->fields.cooldownSecondsRemaining <= 0 && ImGui::Button("Shift"))
						{
							if (IsInGame())
								State.rpcQueue.push(new CmdCheckShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new CmdCheckShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
						}
					}

					if (ImGui::Button("Shift Everyone To"))
					{
						for (auto player : GetAllPlayerControl()) {
							if (IsInGame()) {
								State.rpcQueue.push(new RpcShapeshift(player, State.selectedPlayer, !State.AnimationlessShapeshift));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcShapeshift(player, State.selectedPlayer, !State.AnimationlessShapeshift));
							}
						}
					}
					
					if (ImGui::Button("Unshift Everyone"))
					{
						for (auto player : GetAllPlayerControl()) {
							if (IsInGame()) {
								State.rpcQueue.push(new RpcShapeshift(player, PlayerSelection(player), !State.AnimationlessShapeshift));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcShapeshift(player, PlayerSelection(player), !State.AnimationlessShapeshift));
							}
						}
					}

					if (IsHost())
					{
						if (ImGui::Button("Protect Player")) {
							app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer));
							auto colorId = outfit->fields.ColorId;
							if (IsInGame())
								State.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, State.selectedPlayer, colorId));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, State.selectedPlayer, colorId));
						}
					}
					else if (role == RoleTypes__Enum::GuardianAngel) {
						app::GuardianAngelRole* guardianAngelRole = (app::GuardianAngelRole*)playerRole;
						if (guardianAngelRole->fields.cooldownSecondsRemaining <= 0 && ImGui::Button("Protect Player")) {
							if (IsInGame())
								State.rpcQueue.push(new CmdCheckProtect(*Game::pLocalPlayer, State.selectedPlayer));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new CmdCheckProtect(*Game::pLocalPlayer, State.selectedPlayer));
						}
					}

					if (!selectedPlayer.is_LocalPlayer()) {
						if (ImGui::Button("Teleport To")) {
							if(IsInGame())
								State.rpcQueue.push(new RpcSnapTo(GetTrueAdjustedPosition(selectedPlayer.get_PlayerControl())));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcSnapTo(GetTrueAdjustedPosition(selectedPlayer.get_PlayerControl())));
						}
					}
					ImGui::SameLine();
					if (!selectedPlayer.is_LocalPlayer()) {
						if (ImGui::Button("Teleport To You")) {
							if (IsInGame())
								State.rpcQueue.push(new RpcForceSnapTo(selectedPlayer.get_PlayerControl(), GetTrueAdjustedPosition(*Game::pLocalPlayer)));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcForceSnapTo(selectedPlayer.get_PlayerControl(), GetTrueAdjustedPosition(*Game::pLocalPlayer)));
						}
					}

					ImGui::NewLine();

					if (IsInGame() && !selectedPlayer.is_Disconnected() && IsInMultiplayerGame())
					{
						auto tasks = GetNormalPlayerTasks(selectedPlayer.get_PlayerControl());

						if (State.RevealRoles && PlayerIsImpostor(selectedPlayer.get_PlayerData()))
						{
							ImGui::TextColored(ImVec4(0.8F, 0.2F, 0.0F, 1.0F), "Fake Tasks:");
						}
						else
						{
							ImGui::Text("Tasks:");
						}

						bool shouldEndListBox = ImGui::ListBoxHeader("###tasks#list", ImVec2(181, 94) * State.dpiScale);

						if (selectedPlayer.get_PlayerControl()->fields.myTasks == nullptr)
						{
							ImGui::Text("ERROR: Could not load tasks.");
						}
						else
						{
							for (auto task : tasks)
							{
								if (task->fields.taskStep == task->fields.MaxStep)
									ImGui::TextColored(ImVec4(0.0F, 1.0F, 0.0F, 1.0F), (std::string(TranslateTaskTypes(task->fields._.TaskType))).c_str());
								else
									ImGui::Text((std::string(TranslateTaskTypes(task->fields._.TaskType))).c_str());
							}
						}
						if (shouldEndListBox)
							ImGui::ListBoxFooter();
					}
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}
		}
	}
}