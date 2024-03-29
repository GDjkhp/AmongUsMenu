#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

RpcMurderPlayer::RpcMurderPlayer(PlayerControl* Player, PlayerControl* target)
{
	this->Player = Player;
	this->target = target;
}

void RpcMurderPlayer::Process()
{
	if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) return;

	if (IsInGame() && !IsInMultiplayerGame()) 
		PlayerControl_RpcMurderPlayer(Player, target, true, NULL);
	else if (target != *Game::pLocalPlayer || IsInGame()) 
		PlayerControl_RpcMurderPlayer(Player, target, true, NULL);
	else {
		for (auto p : GetAllPlayerControl()) {
			if (p != *Game::pLocalPlayer) {
				auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
					uint8_t(RpcCalls__Enum::MurderPlayer), SendOption__Enum::None, p->fields._.OwnerId, NULL);
				MessageExtensions_WriteNetObject(writer, (InnerNetObject*)target, NULL);
				MessageWriter_WriteInt32(writer, int32_t(MurderResultFlags__Enum::Succeeded), NULL);
				InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			}
			GetPlayerData(*Game::pLocalPlayer)->fields.IsDead = true;
		}
		PlayerControl_RpcMurderPlayer(Player, target, true, NULL); // unable to move in lobby
	}
}