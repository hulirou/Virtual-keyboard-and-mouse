#include "mydll.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <initguid.h>
#include <SetupAPI.h>
#include <iostream>
#include <limits>
#pragma comment(lib, "setupapi.lib")

//��������루�����޷���
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TouchScreen CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);





//ֱ����Queue.h���ù�����,���ֶ���һ��
//����������뱨��
typedef struct _KEYBOARD_INPUT_REPORT
{
    uint8_t ReportId;  // ����ID 1
    uint8_t Modifiers;    // ���μ�λͼ,Shift��Ctrl��Alt �����μ���״̬(������ܿ���ͬʱ���¶�������Ǻ���ͨ�������벻ͬ)
    uint8_t Reserved;     // �����ֽ�
    uint8_t ScanCodes[6]; // �洢ͬʱ���µİ���ɨ����(֧����� 6 ������ͬʱ����)
} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;

//����������뱨��
typedef struct _MOUSE_INPUT_REPORT
{
    uint8_t ReportId;       // ����ID 2
    uint8_t Buttons;        // ����
    int8_t  DeltaX;         // x��
    int8_t  DeltaY;         // y��
    int8_t  WheelDelta;     // ����
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//���崥�������뱨��
typedef struct _TOUCHSCREEN_INPUT_REPORT
{
    uint8_t ReportId;           // ����ID
    uint8_t Buttons;            // ����״̬ (bit0-bit4��Ӧ5��������bit5-bit7Ϊ���λ)
    uint16_t DeltaX;            // X�����λ�ƣ�0~65535��2�ֽڣ�
    uint16_t DeltaY;            // Y�����λ�ƣ�0~65535��2�ֽڣ�
} TOUCHSCREEN_INPUT_REPORT, * PTOUCHSCREEN_INPUT_REPORT;


// ���巵�ؽṹ�壬����������Ƿ�Ϊ��д�ı�־
typedef struct {
    const char* button;         // ԭ���밴��
    const char* isShift;        // �Ƿ��Сд
    uint8_t keyCode;            // ��ֵ��
    bool isBig;                 // �Ƿ��д0x00=Сд��0x02=��д
    bool isModifier;            // �Ƿ������η�
} KeyInfo;








//ȫ�ֱ�����¼�����Ƿ���
KeyInfo keyboardPressInfo[6] = { NULL };
uint8_t modifiersPressInfo = 0x00;
uint8_t mousePressInfo = 0x00;
// ȫ�ֱ�������������������ʹ���豸���
HANDLE g_hDevice = INVALID_HANDLE_VALUE;






//���̵ļ�ֵת��
KeyInfo keyboardToKeyCode(const char* keyStr) {
    // ��ʼ������ֵ������isShiftĬ��ֵ"null"
    KeyInfo result = { keyStr, "null", 0x00, false,false };

    // �����ַ���������ĸ�����֡����ţ�
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
            return result;  // δ֪���ַ�������Ĭ��ֵ
        }
    }
    // ������ַ������,��Shift״̬
    else {
        if (_stricmp(keyStr, "Ctrl") == 0) {
            result.keyCode = 0x01;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "��Ctrl") == 0) {
            result.keyCode = 0x10;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "Shift") == 0) {
            result.keyCode = 0x02;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "��Shift") == 0) {
            result.keyCode = 0x20;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "Alt") == 0) {
            result.keyCode = 0x04;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "��Alt") == 0) {
            result.keyCode = 0x40;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "GUI") == 0) {
            result.keyCode = 0x08;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "��GUI") == 0) {
            result.keyCode = 0x80;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "win") == 0) {
            result.keyCode = 0x08;
            result.isModifier = true;
        }
        else if (_stricmp(keyStr, "��win") == 0) {
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

//���ļ�ֵת��
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


//�����Ӧ��λ�Ƿ񱻰���
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



// д����̰���״̬(���η�ֱ��ִ�м��㺯������ťִ�����������)
bool addKeyboardPress(const char* str) {


    if (keyboardToKeyCode(str).isModifier == true) {
        addModifierPress(keyboardToKeyCode(str).keyCode);
        return true;
    }
    else {
        // 1. ����Ƿ��Ѵ�����ͬ�������Ƚ�str��ṹ���button��
        for (int i = 0; i < 6; i++) {
            // ���ж�button�Ƿ�ΪNULL�������ָ�����
            if (keyboardPressInfo[i].button != NULL && _stricmp(keyboardPressInfo[i].button, str) == 0) {
                //wprintf(L"�ð����Ѱ���: %s\n", str);
                return false;
            }
        }

        // 2. Ѱ�ҿ�λ�����ת�����KeyInfo
        for (int i = 0; i < 6; i++) {
            if (keyboardPressInfo[i].button == NULL) {  // ��λ�ж�
                keyboardPressInfo[i] = keyboardToKeyCode(str);
                //wprintf(L"��������״̬�Ѽ�¼: %s\n", str);
                return true;
            }
        }

        wprintf(L"���µİ����Ѵ�����\n");
        return false;
    }
}


// �Ƴ����̰���״̬(�����Ƿ������η�)
bool removeKeyboardPress(const char* str) {

    if (keyboardToKeyCode(str).isModifier == true) {
        removeModifierPress(keyboardToKeyCode(str).keyCode);
        return true;
    }
    else {
        for (int i = 0; i < 6; i++) {
            // ���ж�button�Ƿ�ΪNULL�������ָ�����
            if (keyboardPressInfo[i].button != NULL &&
                _stricmp(keyboardPressInfo[i].button, str) == 0) {  // ���ıȽ��߼�
                // ��ո�λ�õĽṹ�壨�ָ���ʼ״̬��
                keyboardPressInfo[i].button = NULL;
                keyboardPressInfo[i].keyCode = 0x00;
                keyboardPressInfo[i].isBig = false;
                //wprintf(L"ɾ�������ɹ�: %s\n", str);
                return true;
            }
        }

        wprintf(L"�ð���δ���£��޷�ɾ��: %s\n", str);
        return false;
    }
}




















//��ȡ�豸����Ͷ������utf-8
void init() {

    //���utf-8�ÿ��ַ������pythonҲ���������
    setlocale(LC_ALL, "en_US.UTF-8"); // �ؼ���ָ���ն˱���Ϊ UTF-8



    //������������Ҫ֪����ʹ��GUID�������з����豸ȡ����һ�����豸·��
    LPTSTR DevicePath = NULL;
    HDEVINFO hInfo = SetupDiGetClassDevs(&DEVICEINTERFACE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (hInfo == NULL) {
        wprintf(L"��ȡ�豸��Ϣ�б�ʧ��: %d\n", GetLastError());
        return;
    }


    SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
    ifdata.cbSize = sizeof(ifdata);
    if (!SetupDiEnumDeviceInterfaces(hInfo, NULL, &DEVICEINTERFACE, 0, &ifdata)) {
        wprintf(L"ö�ٽӿ�ʧ��%d\n", GetLastError());
        SetupDiDestroyDeviceInfoList(hInfo);  // <-- �޸ģ������Դ�ͷ�
        return;
    }

    PSP_DEVICE_INTERFACE_DETAIL_DATA pdetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(1024);
    DWORD dwSize = 0;
    RtlZeroMemory(pdetailData, 1024);
    pdetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    if (!SetupDiGetDeviceInterfaceDetail(hInfo, &ifdata, pdetailData, 1024, &dwSize, NULL)) {
        free(pdetailData);  // <-- �޸ģ������Դ�ͷ�
        SetupDiDestroyDeviceInfoList(hInfo);  // <-- �޸ģ������Դ�ͷ�
        wprintf(L"��ȡ�豸�ӿ�����ʧ��: %d\n", GetLastError());
        return;
    }

    // �����豸·����������ָ��
    DevicePath = (LPTSTR)LocalAlloc(LPTR, (lstrlen(pdetailData->DevicePath) + 1) * sizeof(TCHAR));  // <-- �޸ģ�����·��
    lstrcpy(DevicePath, pdetailData->DevicePath);
    free(pdetailData);
    SetupDiDestroyDeviceInfoList(hInfo);  // <-- �޸ģ��ͷ��豸��Ϣ�б�


    // �ٸ����豸·����ȡ�豸�������ֵ��ȫ�ֱ���
    if (DevicePath != NULL) {
        wprintf(L"�豸·��%ls\n", DevicePath);
        g_hDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);  // <-- �޸ģ���ȡ�������ֵȫ�ֱ���
        LocalFree(DevicePath);  // <-- �޸ģ��ͷ�·���ڴ�

        if (g_hDevice == INVALID_HANDLE_VALUE) {  // <-- �޸ģ��������Ч��
            wprintf(L"��ȡ�豸���ʧ��: %d\n", GetLastError());
        }
    }
    else {
        wprintf(L"�豸�ӿڻ�ȡʧ��: %d\n", GetLastError());
    }

}


//���ͼ��̱��浽��������
bool sendKeyboardReportToDriver(KEYBOARD_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // �������ݵ��豸
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
//������걨�浽��������
bool sendMouseReportToDriver(MOUSE_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // �������ݵ��豸
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
        wprintf(L"�������ݵ�����ʧ��: %d\n", GetLastError());
        return false;
    }
    else {
        return true;
    }
}
//���ʹ��������浽��������
bool sendTouchScreenReportToDriver(TOUCHSCREEN_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // �������ݵ��豸
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
        wprintf(L"�������ݵ�����ʧ��: %d\n", GetLastError());
        return false;
    }
    else {
        return true;
    }
}





//��Ҫ������
//���̰���
bool keyboardPress(const char* KeyCodes) {
    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    keyboardData.ReportId = 1;

    addKeyboardPress(KeyCodes);

    //����ת����ֵ�뵽����
    for (int i = 0; i <= 5; i++) {
        keyboardData.ScanCodes[i] = keyboardPressInfo[i].keyCode;
        //wprintf(L"��ֵ��[%d] = 0x%02X\n", i, keyboardData.ScanCodes[i]);
    }

    keyboardData.Modifiers = modifiersPressInfo;


    if (!sendKeyboardReportToDriver(keyboardData)) {
        return false;
    }

    return true;
}

//�����ͷ�
bool keyboardRelease(const char* KeyCodes) {
    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    KeyInfo keyInfo = keyboardToKeyCode(KeyCodes);
    keyboardData.ReportId = 1;

    //�����0x00�ͻ�ԭȫ�֣��������ɾ������״̬�������ֵ�����ֵ��������
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

//���¼��̺��ͷź���
bool keyboardPressAndRelease(const char* KeyCodes, int time_ms) {
    bool success = keyboardPress(KeyCodes);
    if (!success) {
        wprintf(L"���Ͱ��¼������뱨��ʧ��\n");
        return false;
    }
    Sleep(time_ms);


    success = keyboardRelease(KeyCodes);
    if (!success) {
        wprintf(L"�����ͷż������뱨��ʧ��\n");
        return false;
    }

    return true;
}

//�����ı�
bool typewrite(const char* keyStr, int time_ms) {
    

    int strLen = strlen(keyStr);
    if (strLen == 0 || keyStr == NULL) {
        wprintf(L"�����ַ���Ϊ��\n");
        return false;
    }

    for (int i = 0; i < strLen; i++) {
        char currentChar = keyStr[i];
        char singleChar[2] = { currentChar, '\0' };
        KeyInfo keyInfo = keyboardToKeyCode(singleChar);

        if (keyInfo.keyCode == 0x00) {
            wprintf(L"����δ֪�ַ� '%c'���������ַ�\n", currentChar);
            continue;
        }
        else {
            wprintf(L"����: '%c'����ֵ��: 0x%02X\n", currentChar, keyInfo.keyCode);
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

// ��ϰ���
bool hotkey(const char* args[]) {
    // ��������е����ݣ����6��������nullptr��ֹͣ
    for (int i = 0; i < 6; i++) {
        // ���������ָ�룬˵�����ݽ���
        if (args[i] == nullptr) {
            break;
        }
        keyboardPress(args[i]);
    }
    Sleep(50);
    for (int i = 0; i < 6; i++) {
        // ���������ָ�룬˵�����ݽ���
        if (args[i] == nullptr) {
            break;
        }
        keyboardRelease(args[i]);
    }
    return true;
}

//����ƶ����
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

//�����ƶ����
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

//�����
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



    //ֻ�ͷŰ��µİ�ť
    removeMousePress(keyCode);
    mouseData.Buttons = mousePressInfo;
    if (!sendMouseReportToDriver(mouseData)) {
        return false;
    }
    return true;
}

//��갴��
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

//����ͷ�
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

//������
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

//�鿴���µİ���
bool checkPress() {
    bool hasKey = false;  // ����Ƿ��а���������
    int keyCount = 0;     // ��¼��������

    // ��ͳ���ж��ٸ���Ч����
    for (int i = 0; i < 6; i++) {
        if (keyboardPressInfo[i].button != NULL) {
            hasKey = true;
            keyCount++;
        }
    }

    // �����Ƿ��а��������ͬ����
    if (hasKey) {
        wprintf(L"���µİ�����");
        bool isFirst = true;
        for (int i = 0; i < 6; i++) {
            if (keyboardPressInfo[i].button != NULL) {
                if (!isFirst) {
                    wprintf(L",");  // ��������ö��ŷָ�
                }
                wprintf(L"%s", keyboardPressInfo[i].button);
                isFirst = false;
            }
        }
        wprintf(L"\n");
    }
    else {
        // û�а���ʱ�����ʾ
        wprintf(L"���̰���û�б�����\n");
    }

    wprintf(L"���η����� : 0x%02X\n", modifiersPressInfo);
    wprintf(L"��갴�� : 0x%02X\n", mousePressInfo);


    return true;
}
