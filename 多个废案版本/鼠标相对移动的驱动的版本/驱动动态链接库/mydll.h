#pragma once
#include <windows.h>

extern "C" _declspec(dllexport) HANDLE getHandleDevice();
extern "C" _declspec(dllexport) bool keyboardPressAndRelease(HANDLE hDevice, const char* KeyCodes, const char* Modifiers, int time_ms);

//最主要的两个功能
extern "C" _declspec(dllexport) bool sendKeyboardReportToDriver(HANDLE hDevice, const char* KeyCodes, const char* Modifiers);
extern "C" _declspec(dllexport) bool sendMouseReportToDriver(HANDLE hDevice, const char* buttonFlags, BYTE deltaX, BYTE deltaY, BYTE wheelDelta);
