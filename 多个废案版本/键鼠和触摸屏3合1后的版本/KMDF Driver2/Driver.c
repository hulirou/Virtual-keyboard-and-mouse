#include <ntddk.h>
#include <wdf.h>
#include "Device.h"
#include "wpprecorder.h"


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath){

    //Ĭ�Ͻ���ɹ�
    NTSTATUS status = STATUS_SUCCESS;
    //���ֳ�ʼ������
    WDF_DRIVER_CONFIG config;
    // ��ʼ��WDF��������
    WDF_DRIVER_CONFIG_INIT(&config, EvtDriverDeviceAdd);

    // ����WDF�������󣬰��µ�wdf��������Driver������(���û��ҪDriver���)
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("ע�������Ŀ��ʧ�ܣ�״̬��: 0x%X\n", status));
    }
    KdPrint(("ע�������Ŀ�ܳɹ�\n"));
End:
    return status;
}