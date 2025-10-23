#pragma once
#include <windows.h>
#include <cstdint>



//初始化功能
extern "C" _declspec(dllexport) void init();



//主要的功能
extern "C" _declspec(dllexport) bool keyboardPressAndRelease(const char* KeyCodes, int time_ms);
extern "C" _declspec(dllexport) bool keyboardPress(const char* KeyCodes);
extern "C" _declspec(dllexport) bool keyboardRelease(const char* KeyCodes);
extern "C" _declspec(dllexport) bool typewrite(const char* keyStr, int time_ms);
extern "C" _declspec(dllexport) bool mouseMove(int8_t deltaX, int8_t deltaY);
extern "C" _declspec(dllexport) bool mouseMoveTo(uint16_t deltaX, uint16_t deltaY);
extern "C" _declspec(dllexport) bool mouseClick(const char* buttons);
extern "C" _declspec(dllexport) bool mousePress(const char* buttons);
extern "C" _declspec(dllexport) bool mouseRelease(const char* buttons);
extern "C" _declspec(dllexport) bool mouseWheel(char wheelDelta);
extern "C" _declspec(dllexport) bool checkPress();


//这个可以使用press代替，在python中把列表转换成c数组很麻烦 （鸡肋）
extern "C" _declspec(dllexport) bool hotkey(const char* args[]);


//初始化（设置UTF-8编码，遍历符合GUID的设备位置，获取设备句柄）
//init
// 
// 
//键盘按下并释放，参数1字符串，参数2按下时长毫秒
//keyboardPressAndRelease
// 
// 键盘按下，参数1字符串
//keyboardPress
// 
// 键盘释放，参数1字符串
//keyboardRelease
// 
// 批量输入字符串，参数1字符串（不包含中文，那是输入法的事）
//typewrite
// 
// 一次按下多个按键，参数n个
//hotkey
// 
// 鼠标相对移动,参数1（x轴正负127），参数2（y轴正负127）
//mouseMove
// 
// 鼠标绝对移动,参数1（x轴0-65535），参数2（y轴0-65535）
//mouseMoveTo
// 
// 鼠标点击，参数1字符串
//mouseClick
// 
// 鼠标按下，参数1字符串
//mousePress
// 
// 鼠标释放，参数1字符串
//mouseRelease

// 鼠标滚轮，参数1（x轴正负127）
//mouseWheel
// 
//查看按下状态，键盘按键最多支持6个，键盘修饰符和鼠标按键可全部同时按下。无需参数
//checkPress
