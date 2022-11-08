#include "pch-il2cpp.h"
#include "main.h"
#include "il2cpp-init.h"
#include <VersionHelpers.h>
#include "crc32.h"
#include <shellapi.h>
#include <iostream>
#include "game.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"
#include "version.h"
#include <fstream>
#include <sstream>
#include "gitparams.h"

// test autoRelease main ver increment

HMODULE hModule;
HANDLE hUnloadEvent;

bool GameVersionCheck() {
	auto modulePath = getModulePath(NULL);
	auto gameAssembly = modulePath.parent_path() / "GameAssembly.dll";

	if (!std::filesystem::exists(gameAssembly)) {
		MessageBox(NULL, L"Unable to locate GameAssembly.dll", L"AmongUsMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}

	return true;
}

void Run(LPVOID lpParam) {

	if (!GameVersionCheck()) {
		fclose(stdout);
		FreeConsole();
		FreeLibraryAndExitThread((HMODULE)lpParam, 0);
		return;
	}
	hModule = (HMODULE)lpParam;
	init_il2cpp();

	DetourInitilization();
}
