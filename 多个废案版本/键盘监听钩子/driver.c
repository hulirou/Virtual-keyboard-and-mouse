#include <ntddk.h>
#include <wdf.h>
#include "driver.h"




NTSTATUS status;

//卸载函数
NTSTATUS UnloadDriver()
{
    DbgPrint("驱动已成功卸载\n");
    return STATUS_SUCCESS;
}




//驱动入口
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    //各种初始化变量
    WDF_DRIVER_CONFIG config;
    // 初始化WDF驱动配置
    WDF_DRIVER_CONFIG_INIT(&config, EvtDriverDeviceAdd);

    // 创建WDF驱动对象，把新的wdf驱动放在Driver变量上(这次没有要Driver句柄)
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("注册驱动的框架失败，状态码: 0x%X\n", status));
    }
    KdPrint(("注册驱动的框架成功\n"));


    return STATUS_SUCCESS;
}
