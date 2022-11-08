#include "pch-il2cpp.h"
#include "_hooks.h"
#include "detours/detours.h"
#include "DirectX.h"
#include <iostream>
#include "main.h"
#include "SignatureScan.hpp"
#include "game.h"

using namespace Game;

bool HookFunction(PVOID* ppPointer, PVOID pDetour, const char* functionName) {
	if (const auto error = DetourAttach(ppPointer, pDetour); error != NO_ERROR) {
		std::cout << "Failed to hook " << functionName << ", error " << error << std::endl;
		return false;
	}
	//std::cout << "Hooked " << functionName << std::endl;
	return true;
}

#define HOOKFUNC(n) if (!HookFunction(&(PVOID&)n, d ## n, #n)) return;

bool UnhookFunction(PVOID* ppPointer, PVOID pDetour, const char* functionName) {
	if (const auto error = DetourDetach(ppPointer, pDetour); error != NO_ERROR) {
		std::cout << "Failed to unhook " << functionName << ", error " << error << std::endl;
		return false;
	}
	//std::cout << "Unhooked " << functionName << std::endl;
	return true;
}

#define UNHOOKFUNC(n) if (!UnhookFunction(&(PVOID&)n, d ## n, #n)) return;

void DetourInitilization() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (SaveManager_GetPurchase != nullptr)
		HOOKFUNC(SaveManager_GetPurchase);
	if (PlayerPurchasesData_GetPurchase != nullptr) // v2022.10.25s
		HOOKFUNC(PlayerPurchasesData_GetPurchase);

	DetourTransactionCommit();
}

void DetourUninitialization()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (SaveManager_GetPurchase != nullptr)
		UNHOOKFUNC(SaveManager_GetPurchase);
	if (PlayerPurchasesData_GetPurchase != nullptr) // v2022.10.25s
		UNHOOKFUNC(PlayerPurchasesData_GetPurchase);

	DetourTransactionCommit();
}