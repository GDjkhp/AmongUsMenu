#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

RpcSnapTo::RpcSnapTo(Vector2 targetVector)
{
	this->targetVector = targetVector;
}

void RpcSnapTo::Process()
{
	CustomNetworkTransform_RpcSnapTo((*Game::pLocalPlayer)->fields.NetTransform, this->targetVector, NULL);
}

// sickomode
RpcForceSnapTo::RpcForceSnapTo(PlayerControl* Player, Vector2 targetVector)
{
	this->Player = Player;
	this->targetVector = targetVector;
}

void RpcForceSnapTo::Process()
{
	if (!PlayerSelection(Player).has_value())
		return;

	CustomNetworkTransform_RpcSnapTo((Player)->fields.NetTransform, this->targetVector, NULL);
}