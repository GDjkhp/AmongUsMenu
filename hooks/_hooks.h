#pragma once

void DetourInitilization();
void DetourUninitialization();

bool dAutoOpenDoor_DoUpdate(AutoOpenDoor* __this, float dt, MethodInfo* method);
//bool dConstants_ShouldFlipSkeld(MethodInfo* method);
void dInnerNetClient_Update(InnerNetClient* __this, MethodInfo* method);
void dAmongUsClient_OnPlayerLeft(AmongUsClient* __this, ClientData* data, DisconnectReasons__Enum reason, MethodInfo* method);
void dCustomNetworkTransform_SnapTo(CustomNetworkTransform* __this, Vector2 position, uint16_t minSid, MethodInfo* method);
bool dStatsManager_get_AmBanned(StatsManager* __this, MethodInfo* method);
float dShipStatus_CalculateLightRadius(ShipStatus* __this, GameData_PlayerInfo* player, MethodInfo* method);
float dStatsManager_get_BanPoints(StatsManager* __this, MethodInfo* method);
float dVent_CanUse(Vent* __this, GameData_PlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method);
int32_t dStatsManager_get_BanMinutesLeft(StatsManager* __this, MethodInfo* method);
void dChatBubble_SetName(ChatBubble* __this, String* playerName, bool isDead, bool voted, Color color, MethodInfo* method);
void dChatController_AddChat(ChatController* __this, PlayerControl* sourcePlayer, String* chatText, bool censor, MethodInfo* method);
void dChatController_SetVisible(ChatController* __this, bool visible, MethodInfo* method);
void dHudManager_Update(HudManager* __this, MethodInfo* method);
Vector3 dCamera_ScreenToWorldPoint(Camera* __this, Vector3 position, MethodInfo* method);
void dKeyboardJoystick_Update(KeyboardJoystick* __this, MethodInfo* method);
void dScreenJoystick_FixedUpdate(ScreenJoystick* __this, MethodInfo* method);
void dMeetingHud_Awake(MeetingHud* __this, MethodInfo* method);
void dMeetingHud_Close(MeetingHud* __this, MethodInfo* method);
void dMeetingHud_Update(MeetingHud* __this, MethodInfo* method);
void dMeetingHud_PopulateResults(MeetingHud* __this, Il2CppArraySize* states, MethodInfo* method);
void dPlainDoor_SetDoorway(PlainDoor* __this, bool open, MethodInfo* method);
void dPlayerControl_CompleteTask(PlayerControl* __this, uint32_t idx, MethodInfo* method);
void dPlayerControl_FixedUpdate(PlayerControl* __this, MethodInfo* method);
void dPlayerControl_MurderPlayer(PlayerControl* __this, PlayerControl* target, MurderResultFlags__Enum resultFlags, MethodInfo* method);
void dPlayerControl_StartMeeting(PlayerControl* __this, GameData_PlayerInfo* target, MethodInfo* method);
void dPlayerControl_RpcSyncSettings(PlayerControl* __this, Byte__Array* optionsByteArray, MethodInfo* method);
void dPlayerControl_HandleRpc(PlayerControl* __this, uint8_t callId, MessageReader* reader, MethodInfo* method);
void dPlayerControl_Shapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method);
void dPlayerControl_ProtectPlayer(PlayerControl* __this, PlayerControl* target, int32_t colorId, MethodInfo* method);
void dRenderer_set_enabled(Renderer* __this, bool value, MethodInfo* method);
void dSceneManager_Internal_ActiveSceneChanged(Scene previousActiveScene, Scene newActiveScene, MethodInfo* method);
void dShipStatus_OnEnable(ShipStatus* __this, MethodInfo* method);
void dPolusShipStatus_OnEnable(PolusShipStatus* __this, MethodInfo* method);
void dVent_EnterVent(Vent* __this, PlayerControl* pc, MethodInfo * method);
void* dVent_ExitVent(Vent* __this, PlayerControl* pc, MethodInfo * method);
void dLobbyBehaviour_Start(LobbyBehaviour* __this, MethodInfo* method);
void dGameObject_SetActive(GameObject* __this, bool value, MethodInfo* method);
void dNoShadowBehaviour_LateUpdate(NoShadowBehaviour* __this, MethodInfo* method);
void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method);
void dAirshipStatus_OnEnable(AirshipStatus* __this, MethodInfo* method);
float dAirshipStatus_CalculateLightRadius(AirshipStatus* __this, GameData_PlayerInfo* player, MethodInfo* method);
void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method);
void dDoorBreakerGame_Start(DoorBreakerGame* __this, MethodInfo* method);
void dDoorCardSwipeGame_Begin(DoorCardSwipeGame* __this, PlayerTask* playerTask, MethodInfo* method);
void dDebug_Log(Object* message, MethodInfo* method);
void dDebug_LogError(Object* message, MethodInfo* method);
void dDebug_LogException(Exception* exception, MethodInfo* method);
void dDebug_LogWarning(Object* message, MethodInfo* method);
void dVersionShower_Start(VersionShower* __this, MethodInfo* method);
void dEOSManager_LoginFromAccountTab(EOSManager* __this, MethodInfo* method);
void dEOSManager_InitializePlatformInterface(EOSManager* __this, MethodInfo* method);
bool dEOSManager_IsFreechatAllowed(EOSManager* __this, MethodInfo* method);
void dEOSManager_UpdatePermissionKeys(EOSManager* __this, void* callback, MethodInfo* method);
void dChatController_Update(ChatController* __this, MethodInfo* method);
void dInnerNetClient_EnqueueDisconnect(InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method);
void dInnerNetClient_DisconnectInternal(InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method);
void dRoleManager_SelectRoles(RoleManager* __this, MethodInfo* method);
//void dRoleManager_AssignRolesForTeam(List_1_GameData_PlayerInfo_* players, RoleOptionsData* opts, RoleTeamTypes__Enum team, int32_t teamMax, Nullable_1_RoleTypes_ defaultRole, MethodInfo* method);
//void dRoleManager_AssignRolesFromList(List_1_GameData_PlayerInfo_* players, int32_t teamMax, List_1_RoleTypes_* roleList, int32_t* rolesAssigned, MethodInfo* method);
void dPlayerPhysics_FixedUpdate(PlayerPhysics* __this, MethodInfo* method);
bool dSaveManager_GetPurchase(String* itemKey, String* bundleKey, MethodInfo* method);
void dPlayerControl_TurnOnProtection(PlayerControl* __this, bool visible, int32_t colorId, int32_t guardianPlayerId, MethodInfo* method);
void dAmongUsClient_OnGameEnd(AmongUsClient* __this, Object* endGameResult, MethodInfo* method);
void dAccountManager_UpdateKidAccountDisplay(AccountManager* __this, MethodInfo* method);
void dPlayerStorageManager_OnReadPlayerPrefsComplete(PlayerStorageManager* __this, void* data, MethodInfo* method);
bool dPlayerPurchasesData_GetPurchase(PlayerPurchasesData* __this, String* itemKey, String* bundleKey, MethodInfo* method);
void dGameOptionsManager_set_CurrentGameOptions(GameOptionsManager* __this, IGameOptions* value, MethodInfo* method);
void dExileController_ReEnableGameplay(ExileController* __this, MethodInfo* method);
void dSabotageSystemType_SetInitialSabotageCooldown(SabotageSystemType* __this, MethodInfo* method);
void dFungleShipStatus_OnEnable(FungleShipStatus* __this, MethodInfo* method);
void dMushroomWallDoor_SetDoorway(MushroomWallDoor* __this, bool open, MethodInfo* method);
void dMushroomDoorSabotageMinigame_Begin(MushroomDoorSabotageMinigame* __this, PlayerTask* task, MethodInfo* method);
void dLadder_SetDestinationCooldown(Ladder* __this, MethodInfo* method);
void dZiplineConsole_SetDestinationCooldown(ZiplineConsole* __this, MethodInfo* method);