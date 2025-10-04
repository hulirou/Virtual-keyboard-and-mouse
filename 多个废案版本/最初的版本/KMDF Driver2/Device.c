#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"


// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);


//键盘的设备描述符
const UCHAR keyboardReportDescriptor[] = {
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        //   Usage (Keyboard)
    0xA1, 0x01,        //   Collection (Application)
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
    0xC0,              //   End Collection
};

//鼠标的设备描述符
const UCHAR mouseReportDescriptor[] = {
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        //   Usage (Mouse)
    0xA1, 0x01,        //   Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x03,        //   Usage Maximum (0x03)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x95, 0x03,        //   Report Count (3)
    0x75, 0x01,        //   Report Size (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x05,        //   Report Size (5)
    0x81, 0x01,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x15, 0x81,        //   Logical Minimum (-127)
    0x25, 0x7F,        //   Logical Maximum (127)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x06,        //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x38,        //   Usage (Wheel)
    0x15, 0x81,        //   Logical Minimum (-127)
    0x25, 0x7F,        //   Logical Maximum (127)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x06,        //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              //   End Collection
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

    //初始化VHF键盘
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(keyboardReportDescriptor), keyboardReportDescriptor);
    //创建vhf键盘设备（第二个参数指的是自定义的设备上下文结构体的子项，提供一个初始化好的配置文件和需要vhf设备需要存放的句柄位置）
    //大概逻辑是设备句柄有设备上下文，设备上下文包含键盘和鼠标的vhf设备句柄
    status = VhfCreate(&vhfConfig, &deviceContext->vhfKeyboardHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf键盘设备创建失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf键盘设备创建成功\n"));

    //启动vhf键盘设备
    status = VhfStart(deviceContext->vhfKeyboardHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf键盘设备启动失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf键盘设备启动成功\n"));






    //初始化VHF鼠标
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(keyboardReportDescriptor), mouseReportDescriptor);
    //设置设备属性，并非强制
    //vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    //vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    //vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    //// Set device attributes for mouse with different product ID
    //vhfConfig.VendorID = 0x045E;  // Microsoft vendor ID
    //vhfConfig.ProductID = 0x0040; // Mouse product ID (different from keyboard)
    //vhfConfig.VersionNumber = 0x0001;

    //创建vhf鼠标设备
    status = VhfCreate(&vhfConfig, &deviceContext->vhfMouseHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf鼠标设备创建失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf鼠标设备创建成功\n"));

    //启动vhf鼠标设备
    status = VhfStart(deviceContext->vhfMouseHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf鼠标设备启动失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf鼠标设备启动成功\n"));




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



