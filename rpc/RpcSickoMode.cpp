#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

//damn im NOT too lazy to add new files

RpcShapeshift::RpcShapeshift(PlayerControl* Player, const PlayerSelection& target, bool animate)
{
	this->Player = Player;
	this->target = target;
	this->animate = animate;
}

void RpcShapeshift::Process()
{
	if (!PlayerSelection(Player).has_value())
		return;

	PlayerControl_RpcShapeshift(Player, target.get_PlayerControl().value_or(nullptr), animate, NULL);
}

CmdCheckShapeshift::CmdCheckShapeshift(PlayerControl* Player, const PlayerSelection& target, bool animate)
{
	this->Player = Player;
	this->target = target;
	this->animate = animate;
}

void CmdCheckShapeshift::Process()
{
	if (!PlayerSelection(Player).has_value())
		return;

	PlayerControl_CmdCheckShapeshift(Player, target.get_PlayerControl().value_or(nullptr), animate, NULL);
}

RpcProtectPlayer::RpcProtectPlayer(PlayerControl* Player, PlayerSelection target, uint8_t color)
{
	this->Player = Player;
	this->target = target;
	this->color = color;
}

void RpcProtectPlayer::Process()
{
	if (!PlayerSelection(Player).has_value())
		return;

	PlayerControl_RpcProtectPlayer(Player, target.get_PlayerControl().value_or(nullptr), color, NULL);
}

CmdCheckProtect::CmdCheckProtect(PlayerControl* Player, PlayerSelection target)
{
	this->Player = Player;
	this->target = target;
}

void CmdCheckProtect::Process()
{
	if (!PlayerSelection(Player).has_value())
		return;

	PlayerControl_CmdCheckProtect(Player, target.get_PlayerControl().value_or(nullptr), NULL);
}

RpcVoteKick::RpcVoteKick(PlayerControl* target)
{
	this->target = target;
}

void RpcVoteKick::Process()
{
	if (!PlayerSelection(target).has_value())
		return;

	VoteBanSystem_CmdAddVote(VoteBanSystem__TypeInfo->static_fields->Instance, target->fields._.OwnerId, NULL);
}