#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <initguid.h>
#include <SetupAPI.h>
#include <iostream>
#pragma comment(lib, "setupapi.lib")



//��������루�е���൫�޷���
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_File CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);

//ֱ����Queue.h���ù�����,���ֶ���һ��
//�����������
typedef struct _KEYBOARD_INPUT_REPORT
{
    BYTE Modifiers;    // ���μ�λͼ,Shift��Ctrl��Alt �����μ���״̬(������ܿ���ͬʱ���¶�������Ǻ���ͨ�������벻ͬ)
    BYTE Reserved;     // �����ֽ�
    BYTE ScanCodes[6];  // �洢ͬʱ���µİ���ɨ����(֧����� 6 ������ͬʱ����)
} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;

//�����������
typedef struct _MOUSE_INPUT_REPORT
{
    BYTE Buttons;           //����
    char DeltaX;            //x��
    char DeltaY;            //y��
    char WheelDelta;        //�ƶ�����
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//��·
void runrunrun() {
    printf("���򼴽���·...\n");
    Sleep(1200);
    printf("run��\n");
}

//��ȡ�豸��λ�ú���
PTSTR GetDevicePath() {
    HDEVINFO hInfo = SetupDiGetClassDevs(&DEVICEINTERFACE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (hInfo == NULL) {
        return NULL;
    }


    SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
    ifdata.cbSize = sizeof(ifdata);
    if (!SetupDiEnumDeviceInterfaces(hInfo, NULL, &DEVICEINTERFACE, 0, &ifdata)) {
        printf("ö�ٽӿ�ʧ��%d\n", GetLastError());
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

//���ͼ��̱��浽��������
bool sendKeyboardReportToDriver(HANDLE hDevice, KEYBOARD_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // �������ݵ��豸
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

//������걨�浽��������
bool sendMouseReportToDriver(HANDLE hDevice, MOUSE_INPUT_REPORT sendData) {
    DWORD dwret;
    char outBuffer[1024] = { 0 };

    // �������ݵ��豸
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
        printf("�������ݵ�����ʧ��: %d\n", GetLastError());
        return false;
    }
    else {
        return true;
    }
}

//���¼��̺��ͷź���
bool keyboardPressAndRelease(HANDLE hDevice, BYTE KeyCodes, BYTE Modifiers, int time_ms) {
    // �������ݵ��豸
    KEYBOARD_INPUT_REPORT keyboardData = { 0 };
    keyboardData.ScanCodes[0] = KeyCodes;
    keyboardData.Modifiers = Modifiers;

    if (!sendKeyboardReportToDriver(hDevice, keyboardData)) {
        return false;
    }


    Sleep(time_ms);
    //�ٷ���һ�οգ����ͷ����а�ť
    keyboardData = { 0 };
    keyboardData.ScanCodes[0] = 0x00;
    keyboardData.Modifiers = 0x00;

    if (!sendKeyboardReportToDriver(hDevice, keyboardData)) {
        return false;
    }

}




bool mouseMove(HANDLE hDevice, BYTE buttonFlags, char deltaX, char deltaY, char wheelDelta) {
    // �������ݵ��豸
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
        printf("�豸·��%ls\n", DevicePath);

        // ���豸���õ����
        HANDLE hDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hDevice == INVALID_HANDLE_VALUE)
        {
            printf("��ʧ��%d\n", GetLastError());
            return -1;
        }


        // ���ӳɹ���ѭ����ʾ�˵�
        while (TRUE)
        {
            // ��ʾ�����˵�
            printf("\n\n\n\n========== ������������Գ��� =========\n");
            printf("1. ���ͼ�������A��2��\n");
            printf("2. �����������ƶ�\n");
            printf("3. �����������ƶ�\n");
            printf("4. ���������\n");
            printf("5. �����ֲ���\n");
            printf("0. �˳�\n");
            printf("========================================\n");
            printf("���в��������ӳ������ִ��\n");
            printf("��ѡ�����: ");

            // ��ȡ�û�ѡ��
            int choice;
            std::wcin >> choice;
            std::wcin.ignore(); // ������뻺����

            // ����ѡ��ִ����Ӧ����
            switch (choice) {
            case 1:
                if (keyboardPressAndRelease(hDevice, 0x04, 0x00, 1000)) {
                    printf("�����������!\n");
                }
                break;
            case 2:
                if (mouseMove(hDevice, 0, -120, 0, 0)) {
                    printf("�������ƶ����!\n");
                }
                break;
            case 3:
                if (mouseMove(hDevice, 0, 120, 0, 0)) {
                    printf("�������ƶ����!\n");
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
                printf("��Ч��ѡ��������!\n");
            }
        }
    }
    else
    {
        printf("�豸�ӿڻ�ȡʧ��: %d\n", GetLastError());
        runrunrun();
    }

    return 0;
}