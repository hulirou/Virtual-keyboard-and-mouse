#include "mydll.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <initguid.h>
#include <SetupAPI.h>
#include <iostream>
#pragma comment(lib, "setupapi.lib")

//定义控制码（有点多余但无妨）
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_File CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)

//驱动的唯一标识，凭此连接
// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);

//直接重Queue.h中拿过来用,保持定义一致
//定义键盘输入
typedef struct _KEYBOARD_INPUT_REPORT
{
    BYTE Modifiers;    // 修饰键位图,Shift、Ctrl、Alt 等修饰键的状态(这个可能可以同时按下多个，但是和普通按键输入不同)
    BYTE Reserved;     // 保留字节
    BYTE ScanCodes[6];  // 存储同时按下的按键扫描码(支持最多 6 个键的同时按下)
} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;

//定义鼠标输入
typedef struct _MOUSE_INPUT_REPORT
{
    BYTE Buttons;           //按键
    BYTE DeltaX;            //x轴
    BYTE DeltaY;            //y轴
    BYTE WheelDelta;       //移动距离
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//读取设备的位置函数
PTSTR GetDevicePath() {
    HDEVINFO hInfo = SetupDiGetClassDevs(&DEVICEINTERFACE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (hInfo == NULL) {
        return NULL;
    }


    SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
    ifdata.cbSize = sizeof(ifdata);
    if (!SetupDiEnumDeviceInterfaces(hInfo, NULL, &DEVICEINTERFACE, 0, &ifdata)) {
        printf("枚举接口失败%d\n", GetLastError());
        return NULL;
    }

    PSP_DEVICE_INTERFACE_DETAIL_DATA pdetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(1024);
    DWORD dwSize = 0;
    RtlZeroMemory(pdetailData, 1024);
    pdetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    if (!SetupDiGetDeviceInterfaceDetail(hInfo, &ifdata, pdetailData, 1024, &dwSize, NULL))
    {
        return NULL;
    }
    else
    {
        return pdetailData->DevicePath;
    }
}

HANDLE getHandleDevice() {
    LPTSTR DevicePath = NULL;
    if ((DevicePath = GetDevicePath()) != NULL)
    {
        printf("driverPath:%ls\n", DevicePath);

        // 打开设备，拿到句柄
        HANDLE hDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hDevice == INVALID_HANDLE_VALUE)
        {
            printf("打开失败%d\n", GetLastError());
            return NULL;
        }
        else {
            printf("ok,we can start our work now\n");
            return hDevice;
        }
    }
    else
    {
        printf("设备接口获取失败: %d\n", GetLastError());
        return NULL;
    }
}







//按键转对应的键值码(并不是扫描码,裂开)
//只有char*能传递字符串，char只有单个字符
BYTE keyboardButtonToKeyCode(const char* keyStr) {
    // 处理单字符按键（字母、数字、符号）
    if (strlen(keyStr) == 1) {
        char c = keyStr[0];
        switch (c) {
            // 字母键
        case 'A': case 'a': return 0x04;
        case 'B': case 'b': return 0x05;
        case 'C': case 'c': return 0x06;
        case 'D': case 'd': return 0x07;
        case 'E': case 'e': return 0x08;
        case 'F': case 'f': return 0x09;
        case 'G': case 'g': return 0x0A;
        case 'H': case 'h': return 0x0B;
        case 'I': case 'i': return 0x0C;
        case 'J': case 'j': return 0x0D;
        case 'K': case 'k': return 0x0E;
        case 'L': case 'l': return 0x0F;
        case 'M': case 'm': return 0x10;
        case 'N': case 'n': return 0x11;
        case 'O': case 'o': return 0x12;
        case 'P': case 'p': return 0x13;
        case 'Q': case 'q': return 0x14;
        case 'R': case 'r': return 0x15;
        case 'S': case 's': return 0x16;
        case 'T': case 't': return 0x17;
        case 'U': case 'u': return 0x18;
        case 'V': case 'v': return 0x19;
        case 'W': case 'w': return 0x1A;
        case 'X': case 'x': return 0x1B;
        case 'Y': case 'y': return 0x1C;
        case 'Z': case 'z': return 0x1D;

            // 数字键（主键盘区）
        case '1': return 0x27;
        case '2': return 0x28;
        case '3': return 0x29;
        case '4': return 0x2A;
        case '5': return 0x2B;
        case '6': return 0x2C;
        case '7': return 0x2D;
        case '8': return 0x2E;
        case '9': return 0x2F;
        case '0': return 0x30;

            // 符号键（主键盘区）
        case '-': case '_': return 0x31;
        case '=': case '+': return 0x32;
        case '[': case '{': return 0x33;
        case ']': case '}': return 0x34;
        case '\\': case '|': return 0x35;
        case ';': case ':': return 0x36;
        case '\'': case '"': return 0x37;
        case '`': case '~': return 0x38;
        case ',': case '<': return 0x39;
        case '.': case '>': return 0x3A;
        case '/': case '?': return 0x3B;

        default: return 0x00;  // 未知单字符
        }
    }
    // 处理多字符特殊键
    else {
        if (strcmp(keyStr, "Tab") == 0) return 0x0F;
        if (strcmp(keyStr, "Enter") == 0) return 0x1C;
        if (strcmp(keyStr, "Escape") == 0) return 0x01;
        if (strcmp(keyStr, "Backspace") == 0) return 0x0E;
        if (strcmp(keyStr, "Space") == 0) return 0x39;
        if (strcmp(keyStr, "Shift") == 0) return 0x2A;
        if (strcmp(keyStr, "Ctrl") == 0) return 0x1D;
        if (strcmp(keyStr, "Alt") == 0) return 0x38;
        if (strcmp(keyStr, "F1") == 0) return 0x3B;
        if (strcmp(keyStr, "F2") == 0) return 0x3C;
        if (strcmp(keyStr, "F3") == 0) return 0x3D;
        if (strcmp(keyStr, "F4") == 0) return 0x3E;
        if (strcmp(keyStr, "F5") == 0) return 0x3F;
        if (strcmp(keyStr, "F6") == 0) return 0x40;
        if (strcmp(keyStr, "F7") == 0) return 0x41;
        if (strcmp(keyStr, "F8") == 0) return 0x42;
        if (strcmp(keyStr, "F9") == 0) return 0x43;
        if (strcmp(keyStr, "F10") == 0) return 0x44;
        if (strcmp(keyStr, "F11") == 0) return 0x57;
        if (strcmp(keyStr, "F12") == 0) return 0x58;
        if (strcmp(keyStr, "Insert") == 0) return 0xD2;
        if (strcmp(keyStr, "Delete") == 0) return 0xD3;
        if (strcmp(keyStr, "Home") == 0) return 0xC7;
        if (strcmp(keyStr, "End") == 0) return 0xCF;
        if (strcmp(keyStr, "PageUp") == 0) return 0xC9;
        if (strcmp(keyStr, "PageDown") == 0) return 0xD1;
        if (strcmp(keyStr, "Up") == 0) return 0xC8;
        if (strcmp(keyStr, "Down") == 0) return 0xD0;
        if (strcmp(keyStr, "Left") == 0) return 0xCB;
        if (strcmp(keyStr, "Right") == 0) return 0xCD;

        //为0x00时为释放
        if (strcmp(keyStr, "Null") == 0) return 0x00;

        return 0x00;  // 未知特殊键
    }
}

BYTE keyboardModifierToKeyCode(const char* modifierStr) {
    // 左Shift和右Shift通常有不同的键码，但功能相同
    if (strcmp(modifierStr, "Shift") == 0 || strcmp(modifierStr, "LeftShift") == 0)
        return 0x2A;
    if (strcmp(modifierStr, "RightShift") == 0)
        return 0x36;

    // 左Ctrl和右Ctrl
    if (strcmp(modifierStr, "Ctrl") == 0 || strcmp(modifierStr, "LeftCtrl") == 0)
        return 0x1D;
    if (strcmp(modifierStr, "RightCtrl") == 0)
        return 0x9D;

    // 左Alt和右Alt(Menu键)
    if (strcmp(modifierStr, "Alt") == 0 || strcmp(modifierStr, "LeftAlt") == 0)
        return 0x38;
    if (strcmp(modifierStr, "RightAlt") == 0 || strcmp(modifierStr, "Menu") == 0)
        return 0xB8;

    // Windows键
    if (strcmp(modifierStr, "Win") == 0 || strcmp(modifierStr, "LeftWin") == 0)
        return 0xDB;
    if (strcmp(modifierStr, "RightWin") == 0)
        return 0xDC;

    // 释放修饰键
    if (strcmp(modifierStr, "Null") == 0)
        return 0x00;

    return 0x00;  // 未知修饰键
}

BYTE mouseButtonToKeyCode(const char* buttonStr) {
    // 鼠标左键
    if (strcmp(buttonStr, "Left") == 0 || strcmp(buttonStr, "left") == 0)
        return 0x01;

    // 鼠标右键
    if (strcmp(buttonStr, "Right") == 0 || strcmp(buttonStr, "right") == 0)
        return 0x02;

    // 鼠标中键
    if (strcmp(buttonStr, "Middle") == 0 || strcmp(buttonStr, "middle") == 0)
        return 0x04;

    // 鼠标侧键
    if (strcmp(buttonStr, "X1") == 0)
        return 0x05;
    if (strcmp(buttonStr, "X2") == 0)
        return 0x06;


    // 释放鼠标键
    if (strcmp(buttonStr, "Null") == 0)
        return 0x00;

    return 0x00;  // 未知鼠标键
}






//发送键盘报告到驱动程序
bool sendKeyboardReportToDriver(HANDLE hDevice, const char* KeyCodes, const char* Modifiers) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };


    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    keyboardData.ScanCodes[0] = keyboardButtonToKeyCode(KeyCodes);
    keyboardData.Modifiers = keyboardModifierToKeyCode(Modifiers);

    

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        hDevice,
        IOCTL_Keyboard,
        &keyboardData,
        sizeof(keyboardData),
        outBuffer,
        sizeof(outBuffer),
        &dwret,
        NULL
    );
    if (!success) {
        return false;
    }
    else {
        return true;
    }
}

//发送鼠标报告到驱动程序
bool sendMouseReportToDriver(HANDLE hDevice, const char* buttonFlags, BYTE deltaX, BYTE deltaY, BYTE wheelDelta) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };
    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.Buttons = mouseButtonToKeyCode(buttonFlags);
    mouseData.DeltaX = deltaX;
    mouseData.DeltaY = deltaY;
    mouseData.WheelDelta = wheelDelta;

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        hDevice,
        IOCTL_Mouse,
        &mouseData,
        sizeof(mouseData),
        outBuffer,
        sizeof(outBuffer),
        &dwret,
        NULL
    );
    if (!success) {
        printf("发送数据到驱动失败: %d\n", GetLastError());
        return false;
    }
    else {
        return true;
    }
}

//按下键盘后释放函数
bool keyboardPressAndRelease(HANDLE hDevice,const char* KeyCodes, const char* Modifiers, int time_ms) {
    // 发送数据到设备

    if (!sendKeyboardReportToDriver(hDevice, KeyCodes, Modifiers)) {
        return false;
    }


    Sleep(time_ms);

    //再发送一次空，即释放所有按钮
    if (!sendKeyboardReportToDriver(hDevice, "Null", "Null")) {
        return false;
    }

}

