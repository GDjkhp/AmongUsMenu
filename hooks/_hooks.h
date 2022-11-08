#pragma once

void DetourInitilization();
void DetourUninitialization();

bool dSaveManager_GetPurchase(String* itemKey, String* bundleKey, MethodInfo* method);
bool dPlayerPurchasesData_GetPurchase(PlayerPurchasesData* __this, String* itemKey, String* bundleKey, MethodInfo* method);
