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
    char DeltaX;            //x轴
    char DeltaY;            //y轴
    char WheelDelta;        //移动距离
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//跑路
void runrunrun() {
    printf("程序即将跑路...\n");
    Sleep(1200);
    printf("run！\n");
}

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

//发送键盘报告到驱动程序
bool sendKeyboardReportToDriver(HANDLE hDevice, KEYBOARD_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        hDevice,
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
bool sendMouseReportToDriver(HANDLE hDevice, MOUSE_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // 发送数据到设备
    BOOL success = DeviceIoControl(
        hDevice,
        IOCTL_Mouse,
        &sendData,
        sizeof(sendData),
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
bool keyboardPressAndRelease(HANDLE hDevice, BYTE KeyCodes, BYTE Modifiers, int time_ms) {
    // 发送数据到设备
    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    keyboardData.ScanCodes[0] = KeyCodes;
    keyboardData.Modifiers = Modifiers;

    if (!sendKeyboardReportToDriver(hDevice, keyboardData)) {
        return false;
    }


    Sleep(time_ms);
    //再发送一次空，即释放所有按钮
    keyboardData = { 0 };
    keyboardData.ScanCodes[0] = 0x00;
    keyboardData.Modifiers = 0x00;

    if (!sendKeyboardReportToDriver(hDevice, keyboardData)) {
        return false;
    }

}




bool mouseMove(HANDLE hDevice, BYTE buttonFlags, char deltaX, char deltaY, char wheelDelta) {
    // 发送数据到设备
    MOUSE_INPUT_REPORT mouseData = { 0 };
    mouseData.Buttons = buttonFlags;
    mouseData.DeltaX = deltaX;
    mouseData.DeltaY = deltaY;
    mouseData.WheelDelta = wheelDelta;

    if (!sendMouseReportToDriver(hDevice, mouseData)) {
        return false;
    }
    return true;
}



bool mouseClick(HANDLE hDevice) {
    return true;
}
bool mouseWheel(HANDLE hDevice) {
    return true;
}



int main() {
    LPTSTR DevicePath = NULL;
    if ((DevicePath = GetDevicePath()) != NULL)
    {
        printf("设备路径%ls\n", DevicePath);

        // 打开设备，拿到句柄
        HANDLE hDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hDevice == INVALID_HANDLE_VALUE)
        {
            printf("打开失败%d\n", GetLastError());
            return -1;
        }


        // 连接成功后循环显示菜单
        while (TRUE)
        {
            // 显示操作菜单
            printf("\n\n\n\n========== 狐狸的驱动测试程序 =========\n");
            printf("1. 发送键盘输入A键2秒\n");
            printf("2. 发送鼠标相对移动\n");
            printf("3. 发送鼠标绝对移动\n");
            printf("4. 发送鼠标点击\n");
            printf("5. 鼠标滚轮测试\n");
            printf("0. 退出\n");
            printf("========================================\n");
            printf("所有操作都将延迟两秒后执行\n");
            printf("请选择操作: ");

            // 读取用户选择
            int choice;
            std::wcin >> choice;
            std::wcin.ignore(); // 清除输入缓冲区

            // 根据选择执行相应操作
            switch (choice) {
            case 1:
                if (keyboardPressAndRelease(hDevice, 0x04, 0x00, 1000)) {
                    printf("长按键盘完成!\n");
                }
                break;
            case 2:
                if (mouseMove(hDevice, 0, -120, 0, 0)) {
                    printf("鼠标相对移动完成!\n");
                }
                break;
            case 3:
                if (mouseMove(hDevice, 0, 120, 0, 0)) {
                    printf("鼠标绝对移动完成!\n");
                }
                break;
            case 4:
                break;
            case 5:
                break;
            case 0:
                runrunrun();
                CloseHandle(hDevice);
                return 0;
            default:
                printf("无效的选择，请重试!\n");
            }
        }
    }
    else
    {
        printf("设备接口获取失败: %d\n", GetLastError());
        runrunrun();
    }

    return 0;
}