#include "pch-il2cpp.h"
#include "utility.h"
#include "gitparams.h"

std::filesystem::path getModulePath(HMODULE hModule) {
	TCHAR buff[MAX_PATH];
	GetModuleFileName(hModule, buff, MAX_PATH);
	return std::filesystem::path(buff);
}