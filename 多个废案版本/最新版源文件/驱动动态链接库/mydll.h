#pragma once
#include <windows.h>
#include <cstdint>



//��ʼ������
extern "C" _declspec(dllexport) void init();



//��Ҫ�Ĺ���
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


//�������ʹ��press���棬��python�а��б�ת����c������鷳 �����ߣ�
extern "C" _declspec(dllexport) bool hotkey(const char* args[]);


//��ʼ��������UTF-8���룬��������GUID���豸λ�ã���ȡ�豸�����
//init
// 
// 
//���̰��²��ͷţ�����1�ַ���������2����ʱ������
//keyboardPressAndRelease
// 
// ���̰��£�����1�ַ���
//keyboardPress
// 
// �����ͷţ�����1�ַ���
//keyboardRelease
// 
// ���������ַ���������1�ַ��������������ģ��������뷨���£�
//typewrite
// 
// һ�ΰ��¶������������n��
//hotkey
// 
// �������ƶ�,����1��x������127��������2��y������127��
//mouseMove
// 
// �������ƶ�,����1��x��0-65535��������2��y��0-65535��
//mouseMoveTo
// 
// �����������1�ַ���
//mouseClick
// 
// ��갴�£�����1�ַ���
//mousePress
// 
// ����ͷţ�����1�ַ���
//mouseRelease

// �����֣�����1��x������127��
//mouseWheel
// 
//�鿴����״̬�����̰������֧��6�����������η�����갴����ȫ��ͬʱ���¡��������
//checkPress
