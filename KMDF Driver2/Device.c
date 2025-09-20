#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"





typedef struct _DEVICE_CONTEXT {
    VHFHANDLE VhfHandle;  // 存储虚拟HID设备的句柄
    // 可添加其他设备相关的状态信息，如配置参数、缓冲区等
} DEVICE_CONTEXT, * PDEVICE_CONTEXT;
// 声明获取设备上下文的宏
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);



const UCHAR g_KeyboardReportDescriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x65,        //   Logical Maximum (101)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x29, 0x65,        //   Usage Maximum (0x65)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};



NTSTATUS status;


// 定义全局变量：仅一次，分配内存，可初始化
WDFDEVICE g_Device = NULL;

NTSTATUS EvtDriverDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    UNREFERENCED_PARAMETER(Driver);
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDFDEVICE device;
    WDFQUEUE queue;
    PDEVICE_CONTEXT deviceContext;
    VHF_CONFIG vhfConfig;
    WDF_IO_QUEUE_CONFIG IoConfig;




    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);


    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("VHF-Device: wdf设备创建设备失败 : 0x%08x\n", status));
        return status;
    }

    deviceContext = DeviceGetContext(device);
    RtlZeroMemory(deviceContext, sizeof(DEVICE_CONTEXT));

    //初始化VHF
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(g_KeyboardReportDescriptor), g_KeyboardReportDescriptor);
    //创建vhf设备
    status = VhfCreate(&vhfConfig, &deviceContext->VhfHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf设备创建失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf设备创建成功\n"));

    //启动vhf设备
    status = VhfStart(deviceContext->VhfHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf设备启动失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf设备启动成功\n"));






    //配置IO队列函数
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&IoConfig, WdfIoQueueDispatchSequential);
    IoConfig.EvtIoDeviceControl = EvtIoDeviceControl;
    status = WdfIoQueueCreate(device, &IoConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    if (!NT_SUCCESS(status)) {
        KdPrint(("IO队列创建失败 0x%08X\n", status));
    }
    else {
        KdPrint(("IO队列创建完成 0x%08X\n", status));
    }

    //创建设备接口
    WdfDeviceCreateDeviceInterface(device, &DEVICEINTERFACE, NULL);

    // 给全局变量g_Device赋值
    g_Device = device;
}



