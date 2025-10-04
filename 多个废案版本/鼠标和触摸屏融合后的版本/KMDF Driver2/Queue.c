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




// ���Ͱ������µĺ���
NTSTATUS keyboardPress(PDEVICE_CONTEXT deviceContext, UCHAR scanCodes, UCHAR modifiers)
{
    NTSTATUS status;
    HID_XFER_PACKET packet = {0};
    KEYBOARD_INPUT_REPORT pressReport = { 0 };


    // ʹ���ⲿ�����ɨ��������η�
    pressReport.ScanCodes[0] = scanCodes;
    pressReport.Modifiers = modifiers;

    // ���Ͱ��±���
    packet.reportBuffer = (PUCHAR)&pressReport;
    packet.reportBufferLen = sizeof(pressReport);//�ͷ�ʱ���λ�ú���һ��
    packet.reportId = 0;

    status = VhfReadReportSubmit(deviceContext->vhfKeyboardHandle, &packet);
    if (!NT_SUCCESS(status)) {
        KdPrint(("���Ͱ������±���ʧ��: 0x%08x\n", status));
        return status;
    }


    return STATUS_SUCCESS;
}


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


    //���뼴�ɵĵ��������
    KdPrint(("Ԥ�Ȳ��Բ��ִ���"));


    switch (IoControlCode) {
    case IOCTL_TEST:
        KdPrint(("�õ�������Ϣ"));
        break;
    case IOCTL_Keyboard:
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

        status = keyboardPress(deviceContext, keyboardData->ScanCodes[0], keyboardData->Modifiers);
        if (!NT_SUCCESS(status)) {
            KdPrint(("û�ܳɹ������豸���뱨��"));
            return;
        }

        break;
    case IOCTL_Mouse:
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


        mouseReport.ReportId = mouseData->ReportId;
        mouseReport.Buttons = mouseData->Buttons;
        //��Χ-127��127
        mouseReport.DeltaX = mouseData->DeltaX;
        mouseReport.DeltaY = mouseData->DeltaY;
        mouseReport.WheelDelta = mouseData->WheelDelta;
        KdPrint(("ģʽ״̬(Buttons): 0x%02X\n", mouseReport.ReportId));  // BYTE��%02X��16���ƣ�ռ2λ��
        KdPrint(("��갴��״̬(Buttons): 0x%02X\n", mouseReport.Buttons));  // BYTE��%02X��16���ƣ�ռ2λ��
        KdPrint(("X��ƫ����(DeltaX): %ld\n", mouseReport.DeltaX));          // LONG��%ld�������ͣ�
        KdPrint(("Y��ƫ����(DeltaY): %ld\n", mouseReport.DeltaY));          // LONG��%ld
        KdPrint(("����ƫ����(WheelDelta): %hd\n", mouseReport.WheelDelta)); // SHORT��%hd�������ͣ�


        packet.reportBuffer = (PUCHAR)&mouseReport;
        packet.reportBufferLen = sizeof(mouseReport);
        packet.reportId = 0;

        status = VhfReadReportSubmit(deviceContext->vhfMouseHandle, &packet);


        if (!NT_SUCCESS(status)) {
            KdPrint(("��ȡ3�����������ʧ�ܣ�״̬: 0x%X\n", status));
            break;
        }
        break;
    case IOCTL_TouchScreen:
        KdPrint(("�õ�����������ָ��"));
        //status = WdfRequestRetrieveInputBuffer(
        //    Request,                  // ����1��I/O�������
        //    InputBufferLength,        // ����2������
        //    &InputBuffer,             // ����3�����������ָ��ĵ�ַ
        //    &InputBufferLengthRet     // ����4�����ʵ�ʻ�ȡ�Ļ���������
        //);
        //PMOUSE_INPUT_REPORT mouseData = (PMOUSE_INPUT_REPORT)InputBuffer;
        //MOUSE_INPUT_REPORT mouseReport = { 0 };
        //HID_XFER_PACKET packet = { 0 };


        //mouseReport.ReportId = mouseData->ReportId;
        //mouseReport.Buttons = mouseData->Buttons;
        ////��Χ-127��127
        //mouseReport.DeltaX = mouseData->DeltaX;
        //mouseReport.DeltaY = mouseData->DeltaY;
        ////mouseReport.WheelDelta = mouseData->WheelDelta;
        //KdPrint(("ģʽ״̬(Buttons): 0x%02X\n", mouseReport.ReportId));  // BYTE��%02X��16���ƣ�ռ2λ��
        //KdPrint(("��갴��״̬(Buttons): 0x%02X\n", mouseReport.Buttons));  // BYTE��%02X��16���ƣ�ռ2λ��
        //KdPrint(("X��ƫ����(DeltaX): %ld\n", mouseReport.DeltaX));          // LONG��%ld�������ͣ�
        //KdPrint(("Y��ƫ����(DeltaY): %ld\n", mouseReport.DeltaY));          // LONG��%ld
        //KdPrint(("����ƫ����(WheelDelta): %hd\n", mouseReport.WheelDelta)); // SHORT��%hd�������ͣ�


        //packet.reportBuffer = (PUCHAR)&mouseReport;
        //packet.reportBufferLen = sizeof(mouseReport);
        //packet.reportId = 0;

        //status = VhfReadReportSubmit(deviceContext->vhfMouseHandle, &packet);


        //if (!NT_SUCCESS(status)) {
        //    KdPrint(("��ȡ3�����������ʧ�ܣ�״̬: 0x%X\n", status));
        //    break;
        //}
        break;
    default:
        KdPrint(("δ֪�Ŀ�����"));

    }
    WdfRequestComplete(Request, STATUS_SUCCESS);
}
