#ifndef QUEUE_H
#define QUEUE_H

#include <ntddk.h>
#include <wdf.h>


// ����ȫ�ֱ�������extern���������ڴ棬����ʼ��
extern WDFDEVICE g_Device;


//�����������
typedef struct _KEYBOARD_INPUT_REPORT
{
    BYTE Modifiers;    // ���μ�λͼ,Shift��Ctrl��Alt �����μ���״̬
    BYTE Reserved;     // �����ֽ�
    BYTE KeyCodes[6];  // �洢ͬʱ���µİ���ɨ����(֧����� 6 ������ͬʱ����)

} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;


//�����������
typedef struct _MOUSE_INPUT_REPORT
{
    BYTE Buttons;      // �������Ǹ�����
    BYTE X;            // ˮƽ������ƶ���
    BYTE Y;            // ��ֱ������ƶ���
    BYTE Wheel;        // �����ֵĹ�����
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;


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