#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"


// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);



//这个应该有专门生成的工具（某个国外的驱动论坛说的）
//键盘，鼠标和触摸屏的设备描述符
const UCHAR keyboard_mouse_touchScreen_ReportDescriptor[] = {
    // 键盘部分 - 报告ID=1
    0x05, 0x01,        //   用法页（通用桌面控制）
    0x09, 0x06,        //   用法（键盘）
    0xA1, 0x01,        //   集合（应用层）
    0x85, 0x01,        //   报告ID（1）
    0x05, 0x07,        //   用法页（键盘/小键盘）
    0x19, 0xE0,        //   用法最小值（0xE0，修饰键起始）
    0x29, 0xE7,        //   用法最大值（0xE7，修饰键结束）
    0x15, 0x00,        //   逻辑最小值（0）
    0x25, 0x01,        //   逻辑最大值（1）
    0x75, 0x01,        //   报告大小（1位）
    0x95, 0x08,        //   报告计数（8个）
    0x81, 0x02,        //   输入（数据、变量、绝对值）
    0x95, 0x01,        //   报告计数（1个）
    0x75, 0x08,        //   报告大小（8位）
    0x81, 0x01,        //   输入（常量）
    0x95, 0x06,        //   报告计数（6个）
    0x75, 0x08,        //   报告大小（8位）
    0x15, 0x00,        //   逻辑最小值（0）
    0x25, 0x65,        //   逻辑最大值（101）
    0x05, 0x07,        //   用法页（键盘/小键盘）
    0x19, 0x00,        //   用法最小值（0x00）
    0x29, 0x65,        //   用法最大值（0x65）
    0x81, 0x00,        //   输入（数据、数组、绝对值）
    0xC0,              //   结束集合（应用层）

    // 鼠标部分 - 报告ID=2
    0x05, 0x01,        //   用法页（通用桌面控制）
    0x09, 0x02,        //   用法（鼠标）
    0xA1, 0x01,        //   集合（应用层）
    0x85, 0x02,        //   报告ID（2）
    0x09, 0x01,        //   用法（指针）
    0xA1, 0x00,        //   集合（物理层）

    // 鼠标按键部分
    0x05, 0x09,        //   用法页（按键）
    0x19, 0x01,        //   用法最小值（0x01）
    0x29, 0x05,        //   用法最大值（0x05）
    0x15, 0x00,        //   逻辑最小值（0）
    0x25, 0x01,        //   逻辑最大值（1）
    0x95, 0x05,        //   报告计数（5个）
    0x75, 0x01,        //   报告大小（1位）
    0x81, 0x02,        //   输入（数据、变量、绝对值）

    // 鼠标填充位
    0x95, 0x01,        //   报告计数（1个）
    0x75, 0x03,        //   报告大小（3位）
    0x81, 0x01,        //   输入（常量）

    // 鼠标X和Y轴移动
    0x05, 0x01,        //   用法页（通用桌面控制）
    0x09, 0x30,        //   用法（X轴）
    0x09, 0x31,        //   用法（Y轴）
    0x15, 0x81,        //   逻辑最小值（-127）
    0x25, 0x7F,        //   逻辑最大值（127）
    0x75, 0x08,        //   报告大小（8位）
    0x95, 0x02,        //   报告计数（2个）
    0x81, 0x06,        //   输入（数据、变量、相对值）

    // 鼠标滚轮
    0x09, 0x38,        //   用法（滚轮）
    0x15, 0x81,        //   逻辑最小值（-127）
    0x25, 0x7F,        //   逻辑最大值（127）
    0x75, 0x08,        //   报告大小（8位）
    0x95, 0x01,        //   报告计数（1个）
    0x81, 0x06,        //   输入（数据、变量、相对值）

    0xC0,              //   结束集合（物理层）
    0xC0,              //   结束集合（应用层）

    // 触摸屏部分 - 报告ID=3
    0x05, 0x01,        //   用法页（通用桌面）
    0x09, 0x02,        //   用法（鼠标）
    0xA1, 0x01,        //   集合（应用程序）
    0x85, 0x03,        //   报告ID（3）
    0x09, 0x01,        //   用法（指针）
    0xA1, 0x00,        //   集合（物理）

    // 触摸屏按钮部分
    0x05, 0x09,        //   用法页（按钮）
    0x19, 0x01,        //   用法最小值（01）
    0x29, 0x05,        //   用法最大值（05）
    0x15, 0x00,        //   逻辑最小值（0）
    0x25, 0x01,        //   逻辑最大值（1）
    0x95, 0x05,        //   报告计数（5）
    0x75, 0x01,        //   报告大小（1）
    0x81, 0x02,        //   输入（数据，变量，绝对）

    // 触摸屏填充位
    0x95, 0x01,        //   报告计数（1）
    0x75, 0x03,        //   报告大小（3）
    0x81, 0x01,        //   输入（常量）

    // 触摸屏绝对坐标X、Y
    0x05, 0x01,        //   用法页（通用桌面）
    0x09, 0x30,        //   用法（X坐标）
    0x09, 0x31,        //   用法（Y坐标）
    0x15, 0x00,        //   逻辑最小值（0）
    0x27, 0xFF, 0xFF, 0x00,0x00,//   逻辑最大值（65535）
    0x75, 0x10,        //   报告大小（16）
    0x95, 0x02,        //   报告计数（2）
    0x81, 0x02,        //   输入（数据，变量，绝对）

    0xC0,              //   结束集合（物理）
    0xC0               //   结束集合（应用程序）
};





//清理vhf设备
VOID CleanupVhf(
    _In_ PDEVICE_CONTEXT DeviceContext
)
{
    if (DeviceContext->vhfHandle != NULL)
    {
        VhfDelete(DeviceContext->vhfHandle, TRUE);
        DeviceContext->vhfHandle = NULL;
    }

    if (DeviceContext->vhfHandle != NULL)
    {
        VhfDelete(DeviceContext->vhfHandle, TRUE);
        DeviceContext->vhfHandle = NULL;
    }
}

//清理函数
VOID EvtDeviceContextCleanup(
    _In_ WDFOBJECT Object
)
{
    PDEVICE_CONTEXT deviceContext;

    deviceContext = DeviceGetContext((WDFDEVICE)Object);
    if (deviceContext != NULL)
    {
        CleanupVhf(deviceContext);
    }
}

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
    UNICODE_STRING symbolicLinkName;



    //初始化设备属性配置
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
    //指定清理函数
    deviceAttributes.EvtCleanupCallback = EvtDeviceContextCleanup;


    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("VHF-Device: wdf设备创建设备失败 : 0x%08x\n", status));
        return status;
    }

    //获取设备上下文并将其所有成员初始化为零
    deviceContext = DeviceGetContext(device);
    RtlZeroMemory(deviceContext, sizeof(DEVICE_CONTEXT));



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

    //创建符号链接
    RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\huli");
    status = WdfDeviceCreateSymbolicLink(device, &symbolicLinkName);


    // 给全局变量g_Device赋值
    g_Device = device;





    //初始化VHF组合设备
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(keyboard_mouse_touchScreen_ReportDescriptor), keyboard_mouse_touchScreen_ReportDescriptor);
    //设置设备属性，并非强制
    //vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    //vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    //vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    //为具有不同产品ID的鼠标设置设备属性
    //vhfConfig.VendorID = 0x045E;  //Microsoft供应商ID
    //vhfConfig.ProductID = 0x0040; //鼠标产品ID（与键盘不同）
    //vhfConfig.VersionNumber = 0x0001;

    //创建vhf鼠标设备
    status = VhfCreate(&vhfConfig, &deviceContext->vhfHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf组合设备创建失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf组合设备创建成功\n"));

    //启动vhf鼠标设备
    status = VhfStart(deviceContext->vhfHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf组合设备启动失败 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf组合设备启动成功\n"));




}