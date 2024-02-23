#pragma once
#include <vector>
#include "utility.h"

namespace HostTab {
	const std::vector<const char*> MAP_NAMES = { "The Skeld", "Mira HQ", "Polus", /*"Dleks",*/ "Airship", "The Fungle" };
	//const std::vector<const char*> IMPOSTOR_AMOUNTS = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15" };
	const std::vector<const char*> ROLE_NAMES = { "Random", "Crewmate", "Scientist", "Engineer", "Imposter", "Shapeshifter" };
	void Render();
	const ptrdiff_t GetRoleCount(RoleType role);
}