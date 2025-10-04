#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <initguid.h>
#include <SetupAPI.h>
#pragma comment(lib, "setupapi.lib")
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_File CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);


typedef struct _KEYBOARD_INPUT_DATA
{
    BYTE VirtualKey;
    BOOL KeyDown;
} KEYBOARD_INPUT_DATA, * PKEYBOARD_INPUT_DATA;

typedef struct _MOUSE_INPUT_DATA
{
    LONG DeltaX;
    LONG DeltaY;
    BYTE ButtonFlags;
    SHORT WheelDelta;
} MOUSE_INPUT_DATA, * PMOUSE_INPUT_DATA;



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




int main() {

    LPTSTR DevicePath = NULL;
    if ((DevicePath = GetDevicePath()) != NULL)
    {
        printf("%ls\n", DevicePath);

        //打开设备,拿到句柄
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


        //连接成功后循环发送内容
        while (TRUE)
        {
            // 读取用户输入
            char input[256];
            printf("请输入要发送的数据（输入q退出）: ");
            fgets(input, sizeof(input), stdin);  // 从控制台读取输入
            input[strcspn(input, "\n")] = 0;    // 去除换行符不然输入q会有换行在后面

            // 退出条件
            if (strcmp(input, "q") == 0) {
                CloseHandle(hDevice);
                break;
            }
            DWORD dwret;
            char outBuffer[1024] = { 0 };
            char inputBuffer[1024] = { 0 };

            // 发送数据到设备
            BOOL success = DeviceIoControl(
                hDevice,
                IOCTL_Keyboard,           // 控制码
                &input,                   // 输入数据缓冲区
                strlen(input) + 1,        // 输入数据大小（C语言中字符串是以空字符'\0'结尾，使用真实长度）
                outBuffer,                // 输出缓冲区（如果需要接收响应）
                sizeof(outBuffer),        // 输出缓冲区大小(希望驱动设备返回数据的最大长度)
                &dwret,                   // 驱动输出给3环程序的数据长度
                NULL
            );
            
            

            if (!success) {
                printf("发送数据失败: %d\n", GetLastError());
            }else {
                printf("数据发送成功\n");
            }

            printf("返回输出%s\n", outBuffer);
            printf("\n");
        }




    }
    else
    {
        printf("设备接口获取失败%d\n", GetLastError());
    }

}