#include <ntddk.h>
#include <wdf.h>
#include <string.h>
#include <vhf.h>
#include "Queue.h"

//�����˿����룬������Ӧ�ò�Ψһ�ɣ�Ӧ�ð�...
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_File CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)



//һ��ɾ���ļ��ķ���
NTSTATUS KernelDeleteFile(PWCHAR file_path)
{

    UNICODE_STRING    filepath = { 0 };
    NTSTATUS          status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES obja = { 0 };

    // ��� ZwDeleteFile ��������
    NTSYSAPI NTSTATUS NTAPI ZwDeleteFile(IN POBJECT_ATTRIBUTES ObjectAttributes);

    RtlInitUnicodeString(&filepath, file_path);


    InitializeObjectAttributes(&obja, &filepath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = ZwDeleteFile(&obja);

    if (!NT_SUCCESS(status))
    {
        DbgPrint("ɾ���ļ�ʧ�ܣ������ǣ� %x\n", status);
    }

    return status;
}

// �Զ����ַ����ָ�������strtok
char* custom_strtok(char* str, const char delimiter, char** context) {
    char* start;

    // ��һ�ε���ʱ����ʼ��������
    if (str != NULL) {
        *context = str;
    }

    // ����������ѵ���β������NULL
    if (*context == NULL || **context == '\0') {
        return NULL;
    }

    // ���������ķָ���
    while (**context != '\0' && **context == delimiter) {
        (*context)++;
    }

    // ����Ѿ����ַ���ĩβ������NULL
    if (**context == '\0') {
        return NULL;
    }

    // ��¼��ǰ token ����ʼλ��
    start = *context;

    // �ҵ���һ���ָ������ַ�����β
    while (**context != '\0' && **context != delimiter) {
        (*context)++;
    }

    // ��������ַ�����β�����ָ����滻Ϊ'\0'����ֹ��ǰtoken
    if (**context != '\0') {
        **context = '\0';
        (*context)++; // �ƶ�����һ���ַ���׼����һ�ε���
    }

    return start;
}


//һ�����ڰ�charת����unchar�ķ���,�ɹ���ѽ�����ظ�outValue��TRUE��ʧ�ܻ��������ԭ���FALSE
BOOLEAN charToUchar(const char* str, UCHAR* outValue)
{
    // ������������Ч��
    if (str == NULL || outValue == NULL || *str == '\0') {
        KdPrint(("��Ч���������\n"));
        return FALSE;
    }

    // �ں�ģʽ��ʹ��RtlCharToInteger����ת��
    ULONG value;
    NTSTATUS status = RtlCharToInteger((PCHAR)str, 0, &value);

    if (!NT_SUCCESS(status)) {
        KdPrint(("�ַ���ת��ʧ��: %s, ������: 0x%X\n", str, status));
        return FALSE;
    }

    // ����Ƿ���UCHAR��Χ��
    if (value > 0xFF) {
        KdPrint(("��ֵ����UCHAR��Χ (0-255): %u\n", value));
        return FALSE;
    }

    *outValue = (UCHAR)value;
    return TRUE;
}

// ����ʾ���ֵ�charת��ΪULONG�ķ�����,�ɹ���ѽ�����ظ�outValue��TRUE��ʧ�ܻ��������ԭ���FALSE
BOOLEAN charToULONG(const char* str, ULONG* outValue)
{
    // ������������Ч��
    if (str == NULL || outValue == NULL || *str == '\0') {
        KdPrint(("��Ч���������\n"));
        return FALSE;
    }

    // �ں�ģʽ��ʹ��RtlCharToInteger����ת��
    NTSTATUS status = RtlCharToInteger((PCHAR)str, 0, outValue);

    if (!NT_SUCCESS(status)) {
        KdPrint(("�ַ���ת��ʧ��: %s, ������: 0x%X\n", str, status));
        return FALSE;
    }

    return TRUE;
}

// ���Ͱ������µĺ���
NTSTATUS keyboardPress(PDEVICE_CONTEXT deviceContext, UCHAR scanCode, UCHAR modifiers)
{
    NTSTATUS status;
    HID_XFER_PACKET packet;
    KEYBOARD_INPUT_REPORT pressReport = { 0 };
    KEYBOARD_INPUT_REPORT releaseReport = { 0 };


    // ʹ���ⲿ�����ɨ��������η�
    pressReport.KeyCodes[0] = scanCode;
    pressReport.Modifiers = modifiers;

    // ���Ͱ��±���
    packet.reportBuffer = (PUCHAR)&pressReport;
    packet.reportBufferLen = sizeof(pressReport);
    packet.reportId = 0;

    status = VhfReadReportSubmit(deviceContext->vhfKeyboardHandle, &packet);
    if (!NT_SUCCESS(status)) {
        KdPrint(("���Ͱ������±���ʧ��: 0x%08x\n", status));
        return status;
    }


    return STATUS_SUCCESS;
}

// ���Ͱ�ť�ͷŵĺ���
NTSTATUS keyboardRelease(PDEVICE_CONTEXT deviceContext)
{
    NTSTATUS status;
    HID_XFER_PACKET packet;
    KEYBOARD_INPUT_REPORT pressReport = { 0 };
    KEYBOARD_INPUT_REPORT releaseReport = { 0 };


    // �����ɿ�����
    packet.reportBuffer = (PUCHAR)&releaseReport;
    packet.reportBufferLen = sizeof(KEYBOARD_INPUT_REPORT);  // ��ȫ�˴�
    packet.reportId = 0;



    status = VhfReadReportSubmit(deviceContext->vhfKeyboardHandle, &packet);
    if (!NT_SUCCESS(status)) {
        KdPrint(("���Ͱ����ͷű���ʧ��: 0x%08x\n", status));
        return status;
    }
    return STATUS_SUCCESS;
}

//���¼��̰�ť���ͷţ��̶�1�����ӳ٣�
NTSTATUS keyboardPressAndRelease(PDEVICE_CONTEXT deviceContext, UCHAR scanCode, UCHAR modifiers, ULONG time)
{
    NTSTATUS status;
    LARGE_INTEGER delay;

    //���°�ť
    status = keyboardPress(deviceContext, scanCode, modifiers);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //����1�����ӳ٣�1���� = 10,000��100���뵥λ����ֵ��ʾ���ʱ�䣩
    //delay.QuadPart = -10000;
    delay.QuadPart = -10000LL * time;  // ʹ��LLȷ��64λ���㣬�������
    status = KeDelayExecutionThread(KernelMode, FALSE, &delay);
    //��ʹ�ӳ�ʧ�ܣ��Գ����ͷŰ���
    UNREFERENCED_PARAMETER(status); // �����ӳٵķ���״̬

    //�ͷŰ�ť
    status = keyboardRelease(deviceContext);
    return status;
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


    switch (IoControlCode) {
    case IOCTL_TEST:
        // ��ȡ�û�ģʽ�����͸����������ݻ�����
        //status = WdfRequestRetrieveInputBuffer(
        //    Request,                  // ����1��I/O�������
        //    InputBufferLength,        // ����2������
        //    &InputBuffer,             // ����3�����������ָ��ĵ�ַ
        //    &InputBufferLengthRet     // ����4�����ʵ�ʻ�ȡ�Ļ���������
        //);
        ////KdPrint(("����ϣ�������������ݵ���󳤶��ǣ�%Iu\n", OutputBufferLength));//3��������123��OutputBufferLength��ֵ��1024��InputBufferLength��4
        ////KdPrint(("Ӧ�ó����������ݳ��ȣ�%Iu\n", InputBufferLength));
        //if (!NT_SUCCESS(status)) {
        //    KdPrint(("��ȡ���뻺����ʧ�ܣ�״̬: 0x%X\n", status));
        //    break;
        //}
        //else {
        //    if (InputBuffer != NULL && InputBufferLengthRet > 0) {
        //        KdPrint(("�����ANSI�������ݣ�%s�����ȣ�%Iu�ֽ�\n", InputBuffer, InputBufferLengthRet));
        //    }
        //    else {
        //        KdPrint(("���뻺����Ϊ�ջ򳤶�Ϊ0\n"));
        //    }
        //    //KdPrint(("�����ANSI���ݣ��ֽڼ�����"));
        //    //for (size_t i = 0; i < InputBufferLengthRet; i++) {
        //    //    KdPrint(("%02X ", ((PUCHAR)InputBuffer)[i]));
        //    //}
        //}
        ////��ȡ������Ҫ���ظ��û�ģʽ��������ݻ�����
        //status = WdfRequestRetrieveOutputBuffer(
        //    Request,                  // ����1��I/O�������
        //    OutputBufferLength,       // ����2������
        //    &OutputBuffer,            // ����3�������������ָ�루�ں�ģʽ��ַ��
        //    &OutputBufferLengthRet    // ����4��ʵ�ʿ��õ��������������
        //);
        ////���ƻ���������
        //RtlCopyMemory(OutputBuffer, InputBuffer, InputBufferLength);
        ////����Ӧ�ó���ɹ�����֪������Ϣ��С
        //WdfRequestSetInformation(Request, InputBufferLengthRet);
        ////KdPrint(("������Ϣ��С�ǣ�%Iu �ֽ�\n", InputBufferLengthRet));//Ӧ�ó�������123�õ�4
        ////KdPrint(("���뻺������%s\n", InputBuffer));
        ////KdPrint(("�����������%s\n", OutputBuffer));
        //KernelDeleteFile(L"\\??\\C:\\new.txt");
        //if (!NT_SUCCESS(status)) {
        //    WdfRequestComplete(Request, status);
        //    return;
        //}
        break;
    case IOCTL_Mouse:
        KdPrint(("�õ�������ָ��"));
        // ��ȡ�û�ģʽ�����͸����������ݻ�����
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // ����1��I/O�������
            InputBufferLength,        // ����2������
            &InputBuffer,             // ����3�����������ָ��ĵ�ַ
            &InputBufferLengthRet     // ����4�����ʵ�ʻ�ȡ�Ļ���������
        );
        if (!NT_SUCCESS(status)) {
            KdPrint(("��ȡ���뻺����ʧ�ܣ�״̬: 0x%X\n", status));
            break;
        }


        //��ȡ�ַ����ָ������
        char* context = NULL;
        char* token;
        char result[10][100] = { 0 };
        int i = 0;
        token = custom_strtok(InputBuffer, ',', &context);
        while (token != NULL) {
            // ������ȡ����Ƭ��
            strcpy(result[i], token);
            i++;
            //KdPrint(("��ȡ����Ƭ��: %s\n", token));
            // �������ã���һ��������NULL
            token = custom_strtok(NULL, ',', &context);
        }
        KdPrint(("��ȡ���ĵ�1������(ʹ�ú���): %s\n", result[0]));
        KdPrint(("��ȡ���ĵ�2������(ɨ����): %s\n", result[1]));
        KdPrint(("��ȡ���ĵ�3������(���η�): %s\n", result[2]));
        KdPrint(("��ȡ���ĵ�4������(ʱ��): %s\n", result[3]));



        //���3��������������
        //PMOUSE_INPUT_DATA mouseData = (PMOUSE_INPUT_DATA)inputBuffer;
        //MOUSE_INPUT_REPORT mouseReport = { 0 };
        //HID_XFER_PACKET packet;

        //mouseReport.Buttons = mouseData->ButtonFlags;
        //// Clamp movement to valid range (-127 to 127)
        //mouseReport.X = (BYTE)max(-127, min(127, mouseData->DeltaX));
        //mouseReport.Y = (BYTE)max(-127, min(127, mouseData->DeltaY));
        //mouseReport.Wheel = (BYTE)max(-127, min(127, mouseData->WheelDelta));

        //packet.reportBuffer = (PUCHAR)&mouseReport;
        //packet.reportBufferLen = sizeof(mouseReport);
        //packet.reportId = 0;


        //status = VhfReadReportSubmit(deviceContext->vhfMouseHandle, &packet);


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
        //KdPrint(("����ϣ�������������ݵ���󳤶��ǣ�%Iu\n", OutputBufferLength));//3��������123��OutputBufferLength��ֵ��1024��InputBufferLength��4
        //KdPrint(("Ӧ�ó����������ݳ��ȣ�%Iu\n", InputBufferLength));
        if (!NT_SUCCESS(status)) {
            KdPrint(("��ȡ���뻺����ʧ�ܣ�״̬: 0x%X\n", status));
            break;
        }


        char* context = NULL;
        char* token;
        char result[10][100] = { 0 };
        int i = 0;
        //��һ�ε��ã������ַ����ͷָ���
        token = custom_strtok(InputBuffer, ',', &context);
        while (token != NULL) {
            // ������ȡ����Ƭ��
            strcpy(result[i], token);
            i++;
            //KdPrint(("��ȡ����Ƭ��: %s\n", token));
            // �������ã���һ��������NULL
            token = custom_strtok(NULL, ',', &context);
        }
        KdPrint(("��ȡ���ĵ�1������(ʹ�ú���): %s\n", result[0]));
        KdPrint(("��ȡ���ĵ�2������(ɨ����): %s\n", result[1]));
        KdPrint(("��ȡ���ĵ�3������(���η�): %s\n", result[2]));
        KdPrint(("��ȡ���ĵ�4������(ʱ��): %s\n", result[3]));




        //���3������ļ�������
        UCHAR scanCode;
        UCHAR modifiers;
        ULONG time; 
        if (charToUchar(result[1], &scanCode)&& charToUchar(result[2], &modifiers)&& charToULONG(result[3], &time)) {
            //ɨ��������η�ͬʱת���ɹ�
            if (strcmp(result[0], "keyboardPress") == 0) {
                status = keyboardPress(deviceContext, scanCode, modifiers);
            }
            else if (strcmp(result[0], "keyboardRelease") == 0) {
                status = keyboardRelease(deviceContext);
            }
            else if (strcmp(result[0], "keyboardPressAndRelease") == 0) {
                status = keyboardPressAndRelease(deviceContext, scanCode, modifiers, time);
            }
            else {
                KdPrint(("δ֪�ļ��̲���ָ��: %s\n", result[0]));  // �޸Ĵ�����Ϣ��׼ȷ
                status = STATUS_INVALID_PARAMETER;
            }
        }
        else {
            KdPrint(("ɨ��������η���ʱ��ת��ʧ��\n"));
        }
        break;
    case IOCTL_File:
        KdPrint(("�õ��ļ�����ָ��"));
        break;
    default:
        KdPrint(("δ֪�Ŀ�����"));

    }
    WdfRequestComplete(Request, STATUS_SUCCESS);
}
//&��������ת��Ϊ��ַ������ �� ��ַ����
//* ������ַ��ָ�룩ת��Ϊ����ֵ����ַ �� ��������

// �����ӳ٣�ģ��ʵ�ʰ������µ�ʱ��
//LARGE_INTEGER delay;
//delay.QuadPart = -10000 * 5000;  // 50����
//KeDelayExecutionThread(KernelMode, FALSE, &delay);
