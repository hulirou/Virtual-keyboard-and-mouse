#include <ntddk.h>
#include <wdf.h>
#include <string.h>
#include <vhf.h>
#include "Queue.h"

//定义了控制码，控制码应该不唯一吧，应该吧...
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TouchScreen CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)







VOID EvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,  //应用程序期望驱动返回数据的最大长度
    _In_ size_t InputBufferLength,   //应用程序输入数据长度
    _In_ ULONG IoControlCode         //收到的控制码
)
{
    PVOID InputBuffer = NULL;        // 输入缓冲区指针（用户模式传入）
    size_t InputBufferLengthRet = 0; // 实际获取的输入缓冲区长度
    PVOID OutputBuffer = NULL;       // 输出缓冲区指针（返回给用户模式）
    size_t OutputBufferLengthRet = 0;// 实际获取的输出缓冲区长度
    NTSTATUS status;

    //自定义的设备上下文结构体
    PDEVICE_CONTEXT deviceContext;

    // 1. 先检查全局变量是否有效（避免设备未创建就访问）
    if (g_Device == NULL) {
        KdPrint(("全局设备对象未初始化！\n"));
        WdfRequestComplete(Request, STATUS_DEVICE_NOT_READY);
        return;
    }

    // 2. 通过全局g_Device获取设备上下文（核心目的）
    deviceContext = DeviceGetContext(g_Device);




    switch (IoControlCode) {
    case IOCTL_TEST:
    {
        KdPrint(("得到测试消息"));
        break;
    }
    case IOCTL_Keyboard:
    {
        KdPrint(("得到键盘操作指令"));
        // 获取用户模式程序发送给驱动的数据缓冲区
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // 参数1：I/O请求对象
            InputBufferLength,        // 参数2：？？
            &InputBuffer,             // 参数3：输出缓冲区指针的地址
            &InputBufferLengthRet     // 参数4：输出实际获取的缓冲区长度
        );

        //不太懂，但是把输入的类型转成这个代码中的结构体？
        PKEYBOARD_INPUT_REPORT keyboardData = (PKEYBOARD_INPUT_REPORT)InputBuffer;
        KEYBOARD_INPUT_REPORT keyboardReport = { 0 };
        HID_XFER_PACKET packet = { 0 };


        //写入新的结构体中存放
        keyboardReport.ReportId = keyboardData->ReportId;
        for (int i = 0; i <= 5; i++) {
            keyboardReport.ScanCodes[i] = keyboardData->ScanCodes[i];
        }
        

        keyboardReport.Modifiers = keyboardData->Modifiers;


        for (int i = 0; i <= 5; i++) {
            KdPrint(("按键键值码%d = 0x%02X \n", i, keyboardReport.ScanCodes[i]));
        }

        KdPrint(("修饰符键值码 = 0x%02X \n", keyboardReport.Modifiers));


        //封装 HID 报告传输数据后发送
        packet.reportBuffer = (PUCHAR)&keyboardReport;
        packet.reportBufferLen = sizeof(keyboardReport);//注意：可能的错误，另一个代码释放时这个位置好像不一样
        packet.reportId = 0;
        status = VhfReadReportSubmit(deviceContext->vhfHandle, &packet);



        if (!NT_SUCCESS(status)) {
            KdPrint(("没能成功发送设备输入报告"));
            return;
        }

        break;
    }
    case IOCTL_Mouse:
    {
        KdPrint(("得到鼠标操作指令"));
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // 参数1：I/O请求对象
            InputBufferLength,        // 参数2：？？
            &InputBuffer,             // 参数3：输出缓冲区指针的地址
            &InputBufferLengthRet     // 参数4：输出实际获取的缓冲区长度
        );
        PMOUSE_INPUT_REPORT mouseData = (PMOUSE_INPUT_REPORT)InputBuffer;
        MOUSE_INPUT_REPORT mouseReport = { 0 };
        HID_XFER_PACKET packet = { 0 };


        //写入新的结构体中存放
        mouseReport.ReportId = mouseData->ReportId;
        mouseReport.Buttons = mouseData->Buttons;
        mouseReport.DeltaX = mouseData->DeltaX;
        mouseReport.DeltaY = mouseData->DeltaY;
        mouseReport.WheelDelta = mouseData->WheelDelta;

        KdPrint(("报告ID: 0x%02X\n", mouseReport.ReportId));                // BYTE用%02X（16进制，占2位）
        KdPrint(("鼠标按键状态(Buttons): 0x%02X\n", mouseReport.Buttons));  // BYTE用%02X（16进制，占2位）
        KdPrint(("X轴偏移量(DeltaX): %ld\n", mouseReport.DeltaX));          // LONG用%ld（长整型）
        KdPrint(("Y轴偏移量(DeltaY): %ld\n", mouseReport.DeltaY));          // LONG用%ld
        KdPrint(("滚轮偏移量(WheelDelta): %hd\n", mouseReport.WheelDelta)); // SHORT用%hd（短整型）


        //封装 HID 报告传输数据后发送
        packet.reportBuffer = (PUCHAR)&mouseReport;
        packet.reportBufferLen = sizeof(mouseReport);
        packet.reportId = 0;
        status = VhfReadReportSubmit(deviceContext->vhfHandle, &packet);

        if (!NT_SUCCESS(status)) {
            KdPrint(("没能成功发送设备输入报告\n"));
            return;
        }
        break;
    }
    case IOCTL_TouchScreen:
    {
        KdPrint(("得到触摸屏操作指令\n"));
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // 参数1：I/O请求对象
            InputBufferLength,        // 参数2：？？
            &InputBuffer,             // 参数3：输出缓冲区指针的地址
            &InputBufferLengthRet     // 参数4：输出实际获取的缓冲区长度
        );
        PTOUCHSCREEN_INPUT_REPORT touchScreenData = (PTOUCHSCREEN_INPUT_REPORT)InputBuffer;
        TOUCHSCREEN_INPUT_REPORT touchScreenReport = { 0 };
        HID_XFER_PACKET packet = { 0 };


        //写入新的结构体中存放
        touchScreenReport.ReportId = touchScreenData->ReportId;
        touchScreenReport.Buttons = touchScreenData->Buttons;
        touchScreenReport.DeltaX = touchScreenData->DeltaX;
        touchScreenReport.DeltaY = touchScreenData->DeltaY;


        KdPrint(("触摸屏报告 - ReportId: %hhu\n", touchScreenReport.ReportId));  // 输出报告ID
        KdPrint(("触摸屏报告 - Buttons: %hhu\n", touchScreenReport.Buttons));    // 输出按键状态
        KdPrint(("触摸屏报告 - DeltaX: %hu\n", touchScreenReport.DeltaX));      // 输出X坐标
        KdPrint(("触摸屏报告 - DeltaY: %hu\n", touchScreenReport.DeltaY));      // 输出Y坐标


        //封装 HID 报告传输数据后发送
        packet.reportBuffer = (PUCHAR)&touchScreenReport;
        packet.reportBufferLen = sizeof(touchScreenReport);
        packet.reportId = 0;
        status = VhfReadReportSubmit(deviceContext->vhfHandle, &packet);

        if (!NT_SUCCESS(status)) {
            KdPrint(("没能成功发送设备输入报告\n"));
            return;
        }
        break;
    }
    default:
    {
        KdPrint(("未知的控制码"));
        break;
    }
    }
    WdfRequestComplete(Request, STATUS_SUCCESS);
}
