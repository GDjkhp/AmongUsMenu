#include "pch-il2cpp.h"
#include "utility.h"
#include "state.hpp"
#include "game.h"
#include "gitparams.h"
#include "logger.h"
#include "profiler.h"
#include <random>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

std::filesystem::path getModulePath(HMODULE hModule) {
	TCHAR buff[MAX_PATH];
	GetModuleFileName(hModule, buff, MAX_PATH);
	return std::filesystem::path(buff);
}