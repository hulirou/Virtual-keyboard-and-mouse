#ifndef DEVICE_H
#define DEVICE_H
#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>

NTSTATUS EvtDriverDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
);


//无论是Queue.c还是Device.c都要用到这些定义
VOID EvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
);


//设备上下文结构体
typedef struct _DEVICE_CONTEXT
{
    VHFHANDLE vhfKeyboardHandle;
    VHFHANDLE vhfMouseHandle;

} DEVICE_CONTEXT, * PDEVICE_CONTEXT;

//获取设备上下文的方法
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

#endif