#ifndef DEVICE_H
#define DEVICE_H
#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>

NTSTATUS EvtDriverDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
);


//无论是Queue.c还是Device.c都要用到这个定义
VOID EvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
);
#endif