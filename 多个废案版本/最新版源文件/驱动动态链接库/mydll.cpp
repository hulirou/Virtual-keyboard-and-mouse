#include "mydll.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <initguid.h>
#include <SetupAPI.h>
#include <iostream>
#include <limits>
#pragma comment(lib, "setupapi.lib")

//定义控制码（但多无妨）
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TouchScreen CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);





//直接重Queue.h中拿过来用,保持定义一致
//定义键盘输入报告
typedef struct _KEYBOARD_INPUT_REPORT
{
    uint8_t ReportId;  // 报告ID 1
    uint8_t Modifiers;    // 修饰键位图,Shift、Ctrl、Alt 等修饰键的状态(这个可能可以同时按下多个，但是和普通按键输入不同)
    uint8_t Reserved;     // 保留字节
    uint8_t ScanCodes[6]; // 存储同时按下的按键扫描码(支持最多 6 个键的同时按下)
} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;

//定义鼠标输入报告
typedef struct _MOUSE_INPUT_REPORT
{
    uint8_t ReportId;       // 报告ID 2
    uint8_t Buttons;        // 按键
    int8_t  DeltaX;         // x轴
    int8_t  DeltaY;         // y轴
    int8_t  WheelDelta;     // 滚轮
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//定义触摸板输入报告
typedef struct _TOUCHSCREEN_INPUT_REPORT
{
    uint8_t ReportId;           // 报告ID
    uint8_t Buttons;            // 按键状态 (bit0-bit4对应5个按键，bit5-bit7为填充位)
    uint16_t DeltaX;            // X轴绝对位移（0~65535，2字节）
    uint16_t DeltaY;            // Y轴绝对位移（0~65535，2字节）
} TOUCHSCREEN_INPUT_REPORT, * PTOUCHSCREEN_INPUT_REPORT;


// 定义返回结构体，包含键码和是否为大写的标志
typedef struct {
    const char* button;         // 原输入按键
    const char* isShift;        // 是否大小写
    uint8_t keyCode;            // 键值码
    bool isBig;                 // 是否大写0x00=小写，0x02=大写
    bool isModifier;            // 是否是修饰符
} KeyInfo;








//全局变量记录按键是否按下
KeyInfo keyboardPressInfo[6] = { NULL };
uint8_t modifiersPressInfo = 0x00;
uint8_t mousePressInfo = 0x00;
// 全局变量声明，供其他函数使用设备句柄
HANDLE g_hDevice = INVALID_HANDLE_VALUE;






//键盘的键值转换
KeyInfo keyboardToKeyCode(const char* keyStr) {
    // 初始化返回值：补充isShift默认值"null"
    KeyInfo result = { keyStr, "null", 0x00, false,false };

    // 处理单字符按键（字母、数字、符号）
    if (strlen(keyStr) == 1) {
        char c = keyStr[0];
        switch (c) {
        case 'A':
            result.keyCode = 0x04;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'a':
            result.keyCode = 0x04;
            result.isBig = false;
            return result;
        case 'B':
            result.keyCode = 0x05;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'b':
            result.keyCode = 0x05;
            result.isBig = false;
            return result;
        case 'C':
            result.keyCode = 0x06;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'c':
            result.keyCode = 0x06;
            result.isBig = false;
            return result;
        case 'D':
            result.keyCode = 0x07;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'd':
            result.keyCode = 0x07;
            result.isBig = false;
            return result;
        case 'E':
            result.keyCode = 0x08;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'e':
            result.keyCode = 0x08;
            result.isBig = false;
            return result;
        case 'F':
            result.keyCode = 0x09;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'f':
            result.keyCode = 0x09;
            result.isBig = false;
            return result;
        case 'G':
            result.keyCode = 0x0A;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'g':
            result.keyCode = 0x0A;
            result.isBig = false;
            return result;
        case 'H':
            result.keyCode = 0x0B;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'h':
            result.keyCode = 0x0B;
            result.isBig = false;
            return result;
        case 'I':
            result.keyCode = 0x0C;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'i':
            result.keyCode = 0x0C;
            result.isBig = false;
            return result;
        case 'J':
            result.keyCode = 0x0D;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'j':
            result.keyCode = 0x0D;
            result.isBig = false;
            return result;
        case 'K':
            result.keyCode = 0x0E;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'k':
            result.keyCode = 0x0E;
            result.isBig = false;
            return result;
        case 'L':
            result.keyCode = 0x0F;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'l':
            result.keyCode = 0x0F;
            result.isBig = false;
            return result;
        case 'M':
            result.keyCode = 0x10;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'm':
            result.keyCode = 0x10;
            result.isBig = false;
            return result;
        case 'N':
            result.keyCode = 0x11;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'n':
            result.keyCode = 0x11;
            result.isBig = false;
            return result;
        case 'O':
            result.keyCode = 0x12;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'o':
            result.keyCode = 0x12;
            result.isBig = false;
            return result;
        case 'P':
            result.keyCode = 0x13;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'p':
            result.keyCode = 0x13;
            result.isBig = false;
            return result;
        case 'Q':
            result.keyCode = 0x14;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'q':
            result.keyCode = 0x14;
            result.isBig = false;
            return result;
        case 'R':
            result.keyCode = 0x15;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'r':
            result.keyCode = 0x15;
            result.isBig = false;
            return result;
        case 'S':
            result.keyCode = 0x16;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 's':
            result.keyCode = 0x16;
            result.isBig = false;
            return result;
        case 'T':
            result.keyCode = 0x17;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 't':
            result.keyCode = 0x17;
            result.isBig = false;
            return result;
        case 'U':
            result.keyCode = 0x18;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'u':
            result.keyCode = 0x18;
            result.isBig = false;
            return result;
        case 'V':
            result.keyCode = 0x19;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'v':
            result.keyCode = 0x19;
            result.isBig = false;
            return result;
        case 'W':
            result.keyCode = 0x1A;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'w':
            result.keyCode = 0x1A;
            result.isBig = false;
            return result;
        case 'X':
            result.keyCode = 0x1B;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'x':
            result.keyCode = 0x1B;
            result.isBig = false;
            return result;
        case 'Y':
            result.keyCode = 0x1C;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'y':
            result.keyCode = 0x1C;
            result.isBig = false;
            return result;
        case 'Z':
            result.keyCode = 0x1D;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case 'z':
            result.keyCode = 0x1D;
            result.isBig = false;
            return result;

        case '1':
            result.keyCode = 0x1E;
            return result;
        case '!':
            result.keyCode = 0x1E;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '2':
            result.keyCode = 0x1F;
            return result;
        case '@':
            result.keyCode = 0x1F;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '3':
            result.keyCode = 0x20;
            return result;
        case '#':
            result.keyCode = 0x20;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '4':
            result.keyCode = 0x21;
            return result;
        case '$':
            result.keyCode = 0x21;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '5':
            result.keyCode = 0x22;
            return result;
        case '%':
            result.keyCode = 0x22;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '6':
            result.keyCode = 0x23;
            return result;
        case '^':
            result.keyCode = 0x23;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '7':
            result.keyCode = 0x24;
            return result;
        case '&':
            result.keyCode = 0x24;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '8':
            result.keyCode = 0x25;
            return result;
        case '*':
            result.keyCode = 0x25;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '9':
            result.keyCode = 0x26;
            return result;
        case '(':
            result.keyCode = 0x26;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '0':
            result.keyCode = 0x27;
            return result;
        case ')':
            result.keyCode = 0x27;
            result.isBig = true;
            result.isShift = "shift";
            return result;


        case '-':
            result.keyCode = 0x2D;
            return result;
        case '_':
            result.keyCode = 0x2D;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '=':
            result.keyCode = 0x2E;
            return result;
        case '+':
            result.keyCode = 0x2E;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '[':
            result.keyCode = 0x2F;
            return result;
        case '{':
            result.keyCode = 0x2F;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case ']':
            result.keyCode = 0x30;
            return result;
        case '}':
            result.keyCode = 0x30;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '\\':
            result.keyCode = 0x32;
            return result;
        case '|':
            result.keyCode = 0x32;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case ';':
            result.keyCode = 0x33;
            return result;
        case ':':
            result.keyCode = 0x33;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '\'':
            result.keyCode = 0x34;
            return result;
        case '"':
            result.keyCode = 0x34;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '`':
            result.keyCode = 0x35;
            return result;
        case '~':
            result.keyCode = 0x35;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case ',':
            result.keyCode = 0x36;
            return result;
        case '<':
            result.keyCode = 0x36;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '.':
            result.keyCode = 0x37;
            return result;
        case '>':
            result.keyCode = 0x37;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case '/':
            result.keyCode = 0x38;
            return result;
        case '?':
            result.keyCode = 0x38;
            result.isBig = true;
            result.isShift = "shift";
            return result;
        case ' ':
            result.keyCode = 0x2C;
            return result;
        default:
            return result;  // 未知单字符，返回默认值
        }
    }
    // 处理多字符特殊键,无Shift状态
    else {
        if (_stricmp(keyStr, "Ctrl") == 0) {
            result.keyCode = 0x01;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "右Ctrl") == 0) {
            result.keyCode = 0x10;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "Shift") == 0) {
            result.keyCode = 0x02;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "右Shift") == 0) {
            result.keyCode = 0x20;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "Alt") == 0) {
            result.keyCode = 0x04;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "右Alt") == 0) {
            result.keyCode = 0x40;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "GUI") == 0) {
            result.keyCode = 0x08;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "右GUI") == 0) {
            result.keyCode = 0x80;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "win") == 0) {
            result.keyCode = 0x08;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "右win") == 0) {
            result.keyCode = 0x80;
            result.isModifier = true;
        }



        if (_stricmp(keyStr, "Tab") == 0)
            result.keyCode = 0x2B;
        else if (_stricmp(keyStr, "Enter") == 0)
            result.keyCode = 0x28;
        else if (_stricmp(keyStr, "Escape") == 0)
            result.keyCode = 0x29;
        else if (_stricmp(keyStr, "Backspace") == 0)
            result.keyCode = 0x2A;
        else if (_stricmp(keyStr, "Caps Lock") == 0)
            result.keyCode = 0x39;
        else if (_stricmp(keyStr, "Space") == 0)
            result.keyCode = 0x2C;
        else if (_stricmp(keyStr, "F1") == 0)
            result.keyCode = 0x3A;
        else if (_stricmp(keyStr, "F2") == 0)
            result.keyCode = 0x3B;
        else if (_stricmp(keyStr, "F3") == 0)
            result.keyCode = 0x3C;
        else if (_stricmp(keyStr, "F4") == 0)
            result.keyCode = 0x3D;
        else if (_stricmp(keyStr, "F5") == 0)
            result.keyCode = 0x3E;
        else if (_stricmp(keyStr, "F6") == 0)
            result.keyCode = 0x3F;
        else if (_stricmp(keyStr, "F7") == 0)
            result.keyCode = 0x40;
        else if (_stricmp(keyStr, "F8") == 0)
            result.keyCode = 0x41;
        else if (_stricmp(keyStr, "F9") == 0)
            result.keyCode = 0x42;
        else if (_stricmp(keyStr, "F10") == 0)
            result.keyCode = 0x43;
        else if (_stricmp(keyStr, "F11") == 0)
            result.keyCode = 0x44;
        else if (_stricmp(keyStr, "F12") == 0)
            result.keyCode = 0x45;
        else if (_stricmp(keyStr, "PrintScreen") == 0)
            result.keyCode = 0x46;
        else if (_stricmp(keyStr, "ScrollLock") == 0)
            result.keyCode = 0x47;
        else if (_stricmp(keyStr, "Pause") == 0)
            result.keyCode = 0x48;
        else if (_stricmp(keyStr, "Insert") == 0)
            result.keyCode = 0x49;
        else if (_stricmp(keyStr, "Home") == 0)
            result.keyCode = 0x4A;
        else if (_stricmp(keyStr, "PageUp") == 0)
            result.keyCode = 0x4B;
        else if (_stricmp(keyStr, "Delete") == 0)
            result.keyCode = 0x4C;
        else if (_stricmp(keyStr, "End") == 0)
            result.keyCode = 0x4D;
        else if (_stricmp(keyStr, "PageDown") == 0)
            result.keyCode = 0x4E;
        else if (_stricmp(keyStr, "Left") == 0)
            result.keyCode = 0x50;
        else if (_stricmp(keyStr, "Down") == 0)
            result.keyCode = 0x51;
        else if (_stricmp(keyStr, "Up") == 0)
            result.keyCode = 0x52;
        else if (_stricmp(keyStr, "Right") == 0)
            result.keyCode = 0x4F;
        else if (_stricmp(keyStr, "Null") == 0)
            result.keyCode = 0x00;






        return result;
    }
}

//鼠标的键值转换
uint8_t mousueToKeyCode(const char* keyStr) {
    uint8_t mouseInfo = 0x00;
    if (_stricmp(keyStr, "left") == 0) {
        mouseInfo = 0x01;
    }
    else if (_stricmp(keyStr, "right") == 0) {
        mouseInfo = 0x02;
    }
    else if (_stricmp(keyStr, "mid") == 0) {
        mouseInfo = 0x04;
    }
    else if (_stricmp(keyStr, "x1") == 0) {
        mouseInfo = 0x08;
    }
    else if (_stricmp(keyStr, "x2") == 0) {
        mouseInfo = 0x10;
    }
    else {
        mouseInfo = 0x00;
    }
    return mouseInfo;

}


//计算对应的位是否被按下
void addModifierPress(uint8_t modifierKey) {
    modifiersPressInfo = modifiersPressInfo | modifierKey;
}

void removeModifierPress(uint8_t modifierKey) {
    modifiersPressInfo = modifiersPressInfo & ~modifierKey;
}


void addMousePress(uint8_t mouseKey) {
    mousePressInfo = mousePressInfo | mouseKey;
}

void removeMousePress(uint8_t mouseKey) {
    mousePressInfo = mousePressInfo & ~mouseKey;
}



// 写入键盘按键状态(修饰符直接执行计算函数，按钮执行添加至数组)
bool addKeyboardPress(const char* str) {


    if (keyboardToKeyCode(str).isModifier == true) {
        addModifierPress(keyboardToKeyCode(str).keyCode);
        return true;
    }
    else {
        // 1. 检查是否已存在相同按键（比较str与结构体的button）
        for (int i = 0; i < 6; i++) {
            // 先判断button是否为NULL，避免空指针访问
            if (keyboardPressInfo[i].button != NULL && _stricmp(keyboardPressInfo[i].button, str) == 0) {
                //wprintf(L"该按键已按下: %s\n", str);
                return false;
            }
        }

        // 2. 寻找空位并添加转换后的KeyInfo
        for (int i = 0; i < 6; i++) {
            if (keyboardPressInfo[i].button == NULL) {  // 空位判断
                keyboardPressInfo[i] = keyboardToKeyCode(str);
                //wprintf(L"按键按下状态已记录: %s\n", str);
                return true;
            }
        }

        wprintf(L"按下的按键已达上限\n");
        return false;
    }
}


// 移除键盘按键状态(区分是否是修饰符)
bool removeKeyboardPress(const char* str) {

    if (keyboardToKeyCode(str).isModifier == true) {
        removeModifierPress(keyboardToKeyCode(str).keyCode);
        return true;
    }
    else {
        for (int i = 0; i < 6; i++) {
            // 先判断button是否为NULL，避免空指针访问
            if (keyboardPressInfo[i].button != NULL &&
                _stricmp(keyboardPressInfo[i].button, str) == 0) {  // 核心比较逻辑
                // 清空该位置的结构体（恢复初始状态）
                keyboardPressInfo[i].button = NULL;
                keyboardPressInfo[i].keyCode = 0x00;
                keyboardPressInfo[i].isBig = false;
                //wprintf(L"删除按键成功: %s\n", str);
                return true;
            }
        }

        wprintf(L"该按键未按下，无法删除: %s\n", str);
        return false;
    }
}




















//获取设备句柄和定义输出utf-8
void init() {

    //输出utf-8让宽字符串输出python也能中文输出
    setlocale(LC_ALL, "en_US.UTF-8"); // 关键：指定终端编码为 UTF-8



    //看不懂但是需要知道是使用GUID遍历所有符合设备取出第一个的设备路径
    LPTSTR DevicePath = NULL;
    HDEVINFO hInfo = SetupDiGetClassDevs(&DEVICEINTERFACE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (hInfo == NULL) {
        wprintf(L"获取设备信息列表失败: %d\n", GetLastError());
        return;
    }


    SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
    ifdata.cbSize = sizeof(ifdata);
    if (!SetupDiEnumDeviceInterfaces(hInfo, NULL, &DEVICEINTERFACE, 0, &ifdata)) {
        wprintf(L"枚举接口失败%d\n", GetLastError());
        SetupDiDestroyDeviceInfoList(hInfo);  // <-- 修改：添加资源释放
        return;
    }

    PSP_DEVICE_INTERFACE_DETAIL_DATA pdetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(1024);
    DWORD dwSize = 0;
    RtlZeroMemory(pdetailData, 1024);
    pdetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    if (!SetupDiGetDeviceInterfaceDetail(hInfo, &ifdata, pdetailData, 1024, &dwSize, NULL)) {
        free(pdetailData);  // <-- 修改：添加资源释放
        SetupDiDestroyDeviceInfoList(hInfo);  // <-- 修改：添加资源释放
        wprintf(L"获取设备接口详情失败: %d\n", GetLastError());
        return;
    }

    // 复制设备路径避免悬空指针
    DevicePath = (LPTSTR)LocalAlloc(LPTR, (lstrlen(pdetailData->DevicePath) + 1) * sizeof(TCHAR));  // <-- 修改：复制路径
    lstrcpy(DevicePath, pdetailData->DevicePath);
    free(pdetailData);
    SetupDiDestroyDeviceInfoList(hInfo);  // <-- 修改：释放设备信息列表


    // 再根据设备路径获取设备句柄并赋值给全局变量
    if (DevicePath != NULL) {
        wprintf(L"设备路径%ls\n", DevicePath);
        g_hDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);  // <-- 修改：获取句柄并赋值全局变量
        LocalFree(DevicePath);  // <-- 修改：释放路径内存

        if (g_hDevice == INVALID_HANDLE_VALUE) {  // <-- 修改：检查句柄有效性
            wprintf(L"获取设备句柄失败: %d\n", GetLastError());
        }
    }
    else {
        wprintf(L"设备接口获取失败: %d\n", GetLastError());
    }

}


//发送键盘报告到驱动程序
bool sendKeyboardReportToDriver(KEYBOARD_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        g_hDevice,
        IOCTL_Keyboard,
        &sendData,
        sizeof(sendData),
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
bool sendMouseReportToDriver(MOUSE_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        g_hDevice,
        IOCTL_Mouse,
        &sendData,
        sizeof(sendData),
        outBuffer,
        sizeof(outBuffer),
        &dwret,
        NULL
    );
    if (!success) {
        wprintf(L"发送数据到驱动失败: %d\n", GetLastError());
        return false;
    }
    else {
        return true;
    }
}
//发送触摸屏报告到驱动程序
bool sendTouchScreenReportToDriver(TOUCHSCREEN_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        g_hDevice,
        IOCTL_TouchScreen,
        &sendData,
        sizeof(sendData),
        outBuffer,
        sizeof(outBuffer),
        &dwret,
        NULL
    );
    if (!success) {
        wprintf(L"发送数据到驱动失败: %d\n", GetLastError());
        return false;
    }
    else {
        return true;
    }
}





//主要功能区
//键盘按下
bool keyboardPress(const char* KeyCodes) {
    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    keyboardData.ReportId = 1;

    addKeyboardPress(KeyCodes);

    //批量转换键值码到数组
    for (int i = 0; i <= 5; i++) {
        keyboardData.ScanCodes[i] = keyboardPressInfo[i].keyCode;
        //wprintf(L"键值码[%d] = 0x%02X\n", i, keyboardData.ScanCodes[i]);
    }

    keyboardData.Modifiers = modifiersPressInfo;


    if (!sendKeyboardReportToDriver(keyboardData)) {
        return false;
    }

    return true;
}

//键盘释放
bool keyboardRelease(const char* KeyCodes) {
    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    KeyInfo keyInfo = keyboardToKeyCode(KeyCodes);
    keyboardData.ReportId = 1;

    //如果是0x00就还原全局，否则就先删除按下状态后把所有值填入键值码数组中
    if (keyInfo.keyCode == 0x00) {
        memset(keyboardPressInfo, 0, sizeof(keyboardPressInfo));
        modifiersPressInfo = 0x00;
    }
    else {
        removeKeyboardPress(KeyCodes);
        for (int i = 0; i <= 5; i++) {
            keyboardData.ScanCodes[i] = keyboardPressInfo[i].keyCode;
        }

    }


    keyboardData.Modifiers = modifiersPressInfo;


    if (!sendKeyboardReportToDriver(keyboardData)) {
        return false;
    }

}

//按下键盘后释放函数
bool keyboardPressAndRelease(const char* KeyCodes, int time_ms) {
    bool success = keyboardPress(KeyCodes);
    if (!success) {
        wprintf(L"发送按下键盘输入报告失败\n");
        return false;
    }
    Sleep(time_ms);


    success = keyboardRelease(KeyCodes);
    if (!success) {
        wprintf(L"发送释放键盘输入报告失败\n");
        return false;
    }

    return true;
}

//输入文本
bool typewrite(const char* keyStr, int time_ms) {
    

    int strLen = strlen(keyStr);
    if (strLen == 0 || keyStr == NULL) {
        wprintf(L"错误：字符串为空\n");
        return false;
    }

    for (int i = 0; i < strLen; i++) {
        char currentChar = keyStr[i];
        char singleChar[2] = { currentChar, '\0' };
        KeyInfo keyInfo = keyboardToKeyCode(singleChar);

        if (keyInfo.keyCode == 0x00) {
            wprintf(L"错误：未知字符 '%c'，跳过该字符\n", currentChar);
            continue;
        }
        else {
            wprintf(L"按键: '%c'，键值码: 0x%02X\n", currentChar, keyInfo.keyCode);
            if (keyInfo.isBig) {
                keyboardPress("shift");
            }
            keyboardPressAndRelease(keyInfo.button, time_ms);
            if (keyInfo.isBig) {
                keyboardRelease("shift");
            }
        }
    }
    return true;
}

// 组合按键
bool hotkey(const char* args[]) {
    // 输出数组中的数据，最多6个，遇到nullptr则停止
    for (int i = 0; i < 6; i++) {
        // 如果遇到空指针，说明数据结束
        if (args[i] == nullptr) {
            break;
        }
        keyboardPress(args[i]);
    }
    Sleep(50);
    for (int i = 0; i < 6; i++) {
        // 如果遇到空指针，说明数据结束
        if (args[i] == nullptr) {
            break;
        }
        keyboardRelease(args[i]);
    }
    return true;
}

//相对移动鼠标
bool mouseMove(int8_t deltaX, int8_t deltaY) {

    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.ReportId = 2;
    mouseData.Buttons = 0x80;
    mouseData.DeltaX = deltaX;
    mouseData.DeltaY = deltaY;
    mouseData.WheelDelta = 0;

    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }
    return true;
}

//绝对移动鼠标
bool mouseMoveTo(uint16_t deltaX, uint16_t deltaY) {

    TOUCHSCREEN_INPUT_REPORT touchScreenData = { 0 };
    touchScreenData.ReportId = 3;
    touchScreenData.Buttons = 0x00;
    touchScreenData.DeltaX = deltaX;
    touchScreenData.DeltaY = deltaY;

    if (!sendTouchScreenReportToDriver(touchScreenData)) {
        return false;
    }
    return true;
}

//鼠标点击
bool mouseClick(const char* buttons) {
    uint8_t keyCode = mousueToKeyCode(buttons);
    addMousePress(keyCode);
    wprintf(L"keyCode: 0x%02X\n", keyCode);


    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.ReportId = 2;
    mouseData.Buttons = mousePressInfo;
    mouseData.DeltaX = 0;
    mouseData.DeltaY = 0;
    mouseData.WheelDelta = 0;

    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }



    //只释放按下的按钮
    removeMousePress(keyCode);
    mouseData.Buttons = mousePressInfo;
    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }
    return true;
}

//鼠标按下
bool mousePress(const char* buttons) {

    uint8_t keyCode = mousueToKeyCode(buttons);
    addMousePress(keyCode);
    wprintf(L"keyCode: 0x%02X\n", keyCode);


    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.ReportId = 2;
    mouseData.Buttons = keyCode;
    mouseData.DeltaX = 0;
    mouseData.DeltaY = 0;
    mouseData.WheelDelta = 0;

    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }
}

//鼠标释放
bool mouseRelease(const char* buttons) {
    uint8_t keyCode = mousueToKeyCode(buttons);
    removeMousePress(keyCode);
    wprintf(L"keyCode: 0x%02X\n", keyCode);


    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.ReportId = 2;
    mouseData.Buttons = 0x00;
    mouseData.DeltaX = 0;
    mouseData.DeltaY = 0;
    mouseData.WheelDelta = 0;

    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }
    return true;
}

//鼠标滚轮
bool mouseWheel(char wheelDelta) {
    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.ReportId = 2;
    mouseData.Buttons = 0x80;
    mouseData.DeltaX = 0;
    mouseData.DeltaY = 0;
    mouseData.WheelDelta = wheelDelta;

    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }
    return true;
}

//查看按下的按键
bool checkPress() {
    bool hasKey = false;  // 标记是否有按键被按下
    int keyCount = 0;     // 记录按键数量

    // 先统计有多少个有效按键
    for (int i = 0; i < 6; i++) {
        if (keyboardPressInfo[i].button != NULL) {
            hasKey = true;
            keyCount++;
        }
    }

    // 根据是否有按键输出不同内容
    if (hasKey) {
        wprintf(L"按下的按键有");
        bool isFirst = true;
        for (int i = 0; i < 6; i++) {
            if (keyboardPressInfo[i].button != NULL) {
                if (!isFirst) {
                    wprintf(L",");  // 多个按键用逗号分隔
                }
                wprintf(L"%s", keyboardPressInfo[i].button);
                isFirst = false;
            }
        }
        wprintf(L"\n");
    }
    else {
        // 没有按键时输出提示
        wprintf(L"键盘按键没有被按下\n");
    }

    wprintf(L"修饰符按键 : 0x%02X\n", modifiersPressInfo);
    wprintf(L"鼠标按键 : 0x%02X\n", mousePressInfo);


    return true;
}
