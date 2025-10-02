#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"


// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);



//这个应该有专门生成的工具（某个国外的驱动论坛说的）
//键盘的设备描述符
const UCHAR keyboardReportDescriptor[] = {
    0x05, 0x01,        //   用法页（通用桌面控制）
    0x09, 0x06,        //   用法（键盘）
    0xA1, 0x01,        //   集合（应用层，标识此为键盘设备的功能集合）
    0x05, 0x07,        //   用法页（键盘/小键盘）
    0x19, 0xE0,        //   用法最小值（0xE0，对应左Ctrl等修饰键起始）
    0x29, 0xE7,        //   用法最大值（0xE7，对应右GUI等修饰键结束）
    0x15, 0x00,        //   逻辑最小值（0，代表按键未按下）
    0x25, 0x01,        //   逻辑最大值（1，代表按键按下）
    0x75, 0x01,        //   报告大小（1位，每个修饰键用1位表示状态）
    0x95, 0x08,        //   报告计数（8个，共8个修饰键）
    0x81, 0x02,        //   输入（数据、变量、绝对值，修饰键状态的输入字段）
    0x95, 0x01,        //   报告计数（1个）
    0x75, 0x08,        //   报告大小（8位）
    0x81, 0x01,        //   输入（数据、数组、绝对值，填充位字段，无实际功能）
    0x95, 0x06,        //   报告计数（6个，支持同时按下6个普通按键）
    0x75, 0x08,        //   报告大小（8位，每个按键用1字节扫描码表示）
    0x15, 0x00,        //   逻辑最小值（0，无按键按下时的扫描码）
    0x25, 0x65,        //   逻辑最大值（101，对应键盘最大有效扫描码）
    0x05, 0x07,        //   用法页（键盘/小键盘）
    0x19, 0x00,        //   用法最小值（0x00，无按键状态）
    0x29, 0x65,        //   用法最大值（0x65，最大有效按键扫描码）
    0x81, 0x00,        //   输入（数据、数组、绝对值，普通按键扫描码的输入字段）
    0xC0,              //   结束集合（结束应用层集合）
};

//鼠标的设备描述符
const UCHAR mouseReportDescriptor[] = {
    0x05, 0x01,        //   用法页（通用桌面控制）
    0x09, 0x02,        //   用法（鼠标）
    0xA1, 0x01,        //   集合（应用层，标识此为鼠标设备的功能集合）
    0x09, 0x01,        //   用法（指针，鼠标的核心功能类型）
    0xA1, 0x00,        //   集合（物理层，代表鼠标的物理实体）
    0x05, 0x09,        //   用法页（按键）
    0x19, 0x01,        //   用法最小值（0x01，对应鼠标左键）
    0x29, 0x03,        //   用法最大值（0x03，对应鼠标中键）
    0x15, 0x00,        //   逻辑最小值（0，按键未按下）
    0x25, 0x01,        //   逻辑最大值（1，按键按下）
    0x95, 0x03,        //   报告计数（3个，支持左、右、中3个按键）
    0x75, 0x01,        //   报告大小（1位，每个按键用1位表示状态）
    0x81, 0x02,        //   输入（数据、变量、绝对值，按键状态的输入字段）
    0x95, 0x01,        //   报告计数（1个）
    0x75, 0x05,        //   报告大小（5位）
    0x81, 0x01,        //   输入（数据、数组、绝对值，填充位字段，凑足1字节）
    0x05, 0x01,        //   用法页（通用桌面控制）
    0x09, 0x30,        //   用法（X轴，鼠标水平方向移动）
    0x09, 0x31,        //   用法（Y轴，鼠标垂直方向移动）
    0x15, 0x81,        //   逻辑最小值（-127，负方向最大移动量）
    0x25, 0x7F,        //   逻辑最大值（127，正方向最大移动量）
    0x75, 0x08,        //   报告大小（8位，每个轴移动量用1字节表示）
    0x95, 0x02,        //   报告计数（2个，对应X、Y两个轴）
    0x81, 0x06,        //   输入（数据、变量、相对值，X/Y轴移动量的输入字段）
    0x09, 0x38,        //   用法（滚轮）
    0x15, 0x81,        //   逻辑最小值（-127，滚轮向下最大滚动量）
    0x25, 0x7F,        //   逻辑最大值（127，滚轮向上最大滚动量）
    0x75, 0x08,        //   报告大小（8位，滚轮移动量用1字节表示）
    0x95, 0x01,        //   报告计数（1个，对应滚轮）
    0x81, 0x06,        //   输入（数据、变量、相对值，滚轮移动量的输入字段）
    0xC0,              //   结束集合（结束物理层集合）
    0xC0,              //   结束集合（结束应用层集合）
};




//清理vhf设备
VOID CleanupVhf(
    _In_ PDEVICE_CONTEXT DeviceContext
)
{
    if (DeviceContext->vhfKeyboardHandle != NULL)
    {
        VhfDelete(DeviceContext->vhfKeyboardHandle, TRUE);
        DeviceContext->vhfKeyboardHandle = NULL;
    }

    if (DeviceContext->vhfMouseHandle != NULL)
    {
        VhfDelete(DeviceContext->vhfMouseHandle, TRUE);
        DeviceContext->vhfMouseHandle = NULL;
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
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(mouseReportDescriptor), mouseReportDescriptor);
    //设置设备属性，并非强制
    //vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    //vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    //vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    //为具有不同产品ID的鼠标设置设备属性
    //vhfConfig.VendorID = 0x045E;  //Microsoft供应商ID
    //vhfConfig.ProductID = 0x0040; //鼠标产品ID（与键盘不同）
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




}
