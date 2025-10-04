#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>
#include <ntddk.h>
#include <wdf.h>


// ����ȫ�ֱ�������extern���������ڴ棬����ʼ��
extern WDFDEVICE g_Device;


//����������뱨�棬�����Ҹ������˷�1����,�һ�������ٷ���������ִ�ж������ж�����ִ�У����ֿ��Ը�
typedef struct _KEYBOARD_INPUT_REPORT
{
    uint8_t ReportId;  // ����ID
    uint8_t Modifiers;    // ���μ�λͼ,Shift��Ctrl��Alt �����μ���״̬(������ܿ���ͬʱ���¶�������Ǻ���ͨ�������벻ͬ)
    uint8_t Reserved;     // �����ֽ�
    uint8_t ScanCodes[6]; // �洢ͬʱ���µİ���ɨ����(֧����� 6 ������ͬʱ����)
} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;

//����������뱨��
typedef struct _MOUSE_INPUT_REPORT
{
    uint8_t ReportId;       // ����ID
    uint8_t Buttons;        // ����
    char DeltaX;            // x��
    char DeltaY;            // y��
    char WheelDelta;        // ����
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//���崥�������뱨��
typedef struct _TOUCHSCREEN_INPUT_REPORT
{
    uint8_t ReportId;           // ����ID
    uint8_t Buttons;            // ����״̬ (bit0-bit4��Ӧ5��������bit5-bit7Ϊ���λ)
    uint16_t DeltaX;            // X�����λ�ƣ�0~1023��2�ֽڣ�
    uint16_t DeltaY;            // Y�����λ�ƣ�0~1023��2�ֽڣ�
} TOUCHSCREEN_INPUT_REPORT, * PTOUCHSCREEN_INPUT_REPORT;



//������Queue.c����Device.c��Ҫ�õ���Щ����
VOID EvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
);


//�豸�����Ľṹ��
typedef struct _DEVICE_CONTEXT
{
    VHFHANDLE vhfKeyboardHandle;
    VHFHANDLE vhfMouseHandle;

} DEVICE_CONTEXT, * PDEVICE_CONTEXT;

//��ȡ�豸�����ĵķ���
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

#endif