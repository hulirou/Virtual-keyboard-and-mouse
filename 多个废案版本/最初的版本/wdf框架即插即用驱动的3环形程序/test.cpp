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




int main() {

    LPTSTR DevicePath = NULL;
    if ((DevicePath = GetDevicePath()) != NULL)
    {
        printf("%ls\n", DevicePath);

        //���豸,�õ����
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


        //���ӳɹ���ѭ����������
        while (TRUE)
        {
            // ��ȡ�û�����
            char input[256];
            printf("������Ҫ���͵����ݣ�����q�˳���: ");
            fgets(input, sizeof(input), stdin);  // �ӿ���̨��ȡ����
            input[strcspn(input, "\n")] = 0;    // ȥ�����з���Ȼ����q���л����ں���

            // �˳�����
            if (strcmp(input, "q") == 0) {
                CloseHandle(hDevice);
                break;
            }
            DWORD dwret;
            char outBuffer[1024] = { 0 };
            char inputBuffer[1024] = { 0 };

            // �������ݵ��豸
            BOOL success = DeviceIoControl(
                hDevice,
                IOCTL_Keyboard,           // ������
                &input,                   // �������ݻ�����
                strlen(input) + 1,        // �������ݴ�С��C�������ַ������Կ��ַ�'\0'��β��ʹ����ʵ���ȣ�
                outBuffer,                // ����������������Ҫ������Ӧ��
                sizeof(outBuffer),        // �����������С(ϣ�������豸�������ݵ���󳤶�)
                &dwret,                   // ���������3����������ݳ���
                NULL
            );
            
            

            if (!success) {
                printf("��������ʧ��: %d\n", GetLastError());
            }else {
                printf("���ݷ��ͳɹ�\n");
            }

            printf("�������%s\n", outBuffer);
            printf("\n");
        }




    }
    else
    {
        printf("�豸�ӿڻ�ȡʧ��%d\n", GetLastError());
    }

}