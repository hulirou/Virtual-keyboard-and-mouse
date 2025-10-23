#include <ntddk.h>
#include <wdf.h>
#include <string.h>
#include <vhf.h>
#include "Queue.h"

//�����˿����룬������Ӧ�ò�Ψһ�ɣ�Ӧ�ð�...
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TouchScreen CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)







VOID EvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,  //Ӧ�ó������������������ݵ���󳤶�
    _In_ size_t InputBufferLength,   //Ӧ�ó����������ݳ���
    _In_ ULONG IoControlCode         //�յ��Ŀ�����
)
{
    PVOID InputBuffer = NULL;        // ���뻺����ָ�루�û�ģʽ���룩
    size_t InputBufferLengthRet = 0; // ʵ�ʻ�ȡ�����뻺��������
    PVOID OutputBuffer = NULL;       // ���������ָ�루���ظ��û�ģʽ��
    size_t OutputBufferLengthRet = 0;// ʵ�ʻ�ȡ���������������
    NTSTATUS status;

    //�Զ�����豸�����Ľṹ��
    PDEVICE_CONTEXT deviceContext;

    // 1. �ȼ��ȫ�ֱ����Ƿ���Ч�������豸δ�����ͷ��ʣ�
    if (g_Device == NULL) {
        KdPrint(("ȫ���豸����δ��ʼ����\n"));
        WdfRequestComplete(Request, STATUS_DEVICE_NOT_READY);
        return;
    }

    // 2. ͨ��ȫ��g_Device��ȡ�豸�����ģ�����Ŀ�ģ�
    deviceContext = DeviceGetContext(g_Device);




    switch (IoControlCode) {
    case IOCTL_TEST:
    {
        KdPrint(("�õ�������Ϣ"));
        break;
    }
    case IOCTL_Keyboard:
    {
        KdPrint(("�õ����̲���ָ��"));
        // ��ȡ�û�ģʽ�����͸����������ݻ�����
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // ����1��I/O�������
            InputBufferLength,        // ����2������
            &InputBuffer,             // ����3�����������ָ��ĵ�ַ
            &InputBufferLengthRet     // ����4�����ʵ�ʻ�ȡ�Ļ���������
        );

        //��̫�������ǰ����������ת����������еĽṹ�壿
        PKEYBOARD_INPUT_REPORT keyboardData = (PKEYBOARD_INPUT_REPORT)InputBuffer;
        KEYBOARD_INPUT_REPORT keyboardReport = { 0 };
        HID_XFER_PACKET packet = { 0 };


        //д���µĽṹ���д��
        keyboardReport.ReportId = keyboardData->ReportId;
        for (int i = 0; i <= 5; i++) {
            keyboardReport.ScanCodes[i] = keyboardData->ScanCodes[i];
        }
        

        keyboardReport.Modifiers = keyboardData->Modifiers;


        for (int i = 0; i <= 5; i++) {
            KdPrint(("������ֵ��%d = 0x%02X \n", i, keyboardReport.ScanCodes[i]));
        }

        KdPrint(("���η���ֵ�� = 0x%02X \n", keyboardReport.Modifiers));


        //��װ HID ���洫�����ݺ���
        packet.reportBuffer = (PUCHAR)&keyboardReport;
        packet.reportBufferLen = sizeof(keyboardReport);//ע�⣺���ܵĴ�����һ�������ͷ�ʱ���λ�ú���һ��
        packet.reportId = 0;
        status = VhfReadReportSubmit(deviceContext->vhfHandle, &packet);



        if (!NT_SUCCESS(status)) {
            KdPrint(("û�ܳɹ������豸���뱨��"));
            return;
        }

        break;
    }
    case IOCTL_Mouse:
    {
        KdPrint(("�õ�������ָ��"));
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // ����1��I/O�������
            InputBufferLength,        // ����2������
            &InputBuffer,             // ����3�����������ָ��ĵ�ַ
            &InputBufferLengthRet     // ����4�����ʵ�ʻ�ȡ�Ļ���������
        );
        PMOUSE_INPUT_REPORT mouseData = (PMOUSE_INPUT_REPORT)InputBuffer;
        MOUSE_INPUT_REPORT mouseReport = { 0 };
        HID_XFER_PACKET packet = { 0 };


        //д���µĽṹ���д��
        mouseReport.ReportId = mouseData->ReportId;
        mouseReport.Buttons = mouseData->Buttons;
        mouseReport.DeltaX = mouseData->DeltaX;
        mouseReport.DeltaY = mouseData->DeltaY;
        mouseReport.WheelDelta = mouseData->WheelDelta;

        KdPrint(("����ID: 0x%02X\n", mouseReport.ReportId));                // BYTE��%02X��16���ƣ�ռ2λ��
        KdPrint(("��갴��״̬(Buttons): 0x%02X\n", mouseReport.Buttons));  // BYTE��%02X��16���ƣ�ռ2λ��
        KdPrint(("X��ƫ����(DeltaX): %ld\n", mouseReport.DeltaX));          // LONG��%ld�������ͣ�
        KdPrint(("Y��ƫ����(DeltaY): %ld\n", mouseReport.DeltaY));          // LONG��%ld
        KdPrint(("����ƫ����(WheelDelta): %hd\n", mouseReport.WheelDelta)); // SHORT��%hd�������ͣ�


        //��װ HID ���洫�����ݺ���
        packet.reportBuffer = (PUCHAR)&mouseReport;
        packet.reportBufferLen = sizeof(mouseReport);
        packet.reportId = 0;
        status = VhfReadReportSubmit(deviceContext->vhfHandle, &packet);

        if (!NT_SUCCESS(status)) {
            KdPrint(("û�ܳɹ������豸���뱨��\n"));
            return;
        }
        break;
    }
    case IOCTL_TouchScreen:
    {
        KdPrint(("�õ�����������ָ��\n"));
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // ����1��I/O�������
            InputBufferLength,        // ����2������
            &InputBuffer,             // ����3�����������ָ��ĵ�ַ
            &InputBufferLengthRet     // ����4�����ʵ�ʻ�ȡ�Ļ���������
        );
        PTOUCHSCREEN_INPUT_REPORT touchScreenData = (PTOUCHSCREEN_INPUT_REPORT)InputBuffer;
        TOUCHSCREEN_INPUT_REPORT touchScreenReport = { 0 };
        HID_XFER_PACKET packet = { 0 };


        //д���µĽṹ���д��
        touchScreenReport.ReportId = touchScreenData->ReportId;
        touchScreenReport.Buttons = touchScreenData->Buttons;
        touchScreenReport.DeltaX = touchScreenData->DeltaX;
        touchScreenReport.DeltaY = touchScreenData->DeltaY;


        KdPrint(("���������� - ReportId: %hhu\n", touchScreenReport.ReportId));  // �������ID
        KdPrint(("���������� - Buttons: %hhu\n", touchScreenReport.Buttons));    // �������״̬
        KdPrint(("���������� - DeltaX: %hu\n", touchScreenReport.DeltaX));      // ���X����
        KdPrint(("���������� - DeltaY: %hu\n", touchScreenReport.DeltaY));      // ���Y����


        //��װ HID ���洫�����ݺ���
        packet.reportBuffer = (PUCHAR)&touchScreenReport;
        packet.reportBufferLen = sizeof(touchScreenReport);
        packet.reportId = 0;
        status = VhfReadReportSubmit(deviceContext->vhfHandle, &packet);

        if (!NT_SUCCESS(status)) {
            KdPrint(("û�ܳɹ������豸���뱨��\n"));
            return;
        }
        break;
    }
    default:
    {
        KdPrint(("δ֪�Ŀ�����"));
        break;
    }
    }
    WdfRequestComplete(Request, STATUS_SUCCESS);
}
