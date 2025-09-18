#include <ntddk.h>
#include <wdf.h>
#include <string.h>
#include <vhf.h>
#include "Queue.h"

//定义了控制码，控制码应该不唯一吧，应该吧...
#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Mouse CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Keyboard CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_File CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_ANY_ACCESS)


typedef struct _DEVICE_CONTEXT {
    VHFHANDLE VhfHandle;  // 存储虚拟HID设备的句柄
    // 可添加其他设备相关的状态信息，如配置参数、缓冲区等
} DEVICE_CONTEXT, * PDEVICE_CONTEXT;
// 声明获取设备上下文的宏
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)


//一个删除文件的方法
NTSTATUS KernelDeleteFile(PWCHAR file_path)
{

    UNICODE_STRING    filepath = { 0 };
    NTSTATUS          status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES obja = { 0 };

    // 添加 ZwDeleteFile 函数声明
    NTSYSAPI NTSTATUS NTAPI ZwDeleteFile(IN POBJECT_ATTRIBUTES ObjectAttributes);

    RtlInitUnicodeString(&filepath, file_path);


    InitializeObjectAttributes(&obja, &filepath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = ZwDeleteFile(&obja);

    if (!NT_SUCCESS(status))
    {
        DbgPrint("删除文件失败，错误是： %x\n", status);
    }

    return status;
}

// 自定义字符串分割函数，替代strtok
char* custom_strtok(char* str, const char delimiter, char** context) {
    char* start;

    // 第一次调用时，初始化上下文
    if (str != NULL) {
        *context = str;
    }

    // 如果上下文已到结尾，返回NULL
    if (*context == NULL || **context == '\0') {
        return NULL;
    }

    // 跳过连续的分隔符
    while (**context != '\0' && **context == delimiter) {
        (*context)++;
    }

    // 如果已经到字符串末尾，返回NULL
    if (**context == '\0') {
        return NULL;
    }

    // 记录当前 token 的起始位置
    start = *context;

    // 找到下一个分隔符或字符串结尾
    while (**context != '\0' && **context != delimiter) {
        (*context)++;
    }

    // 如果不是字符串结尾，将分隔符替换为'\0'以终止当前token
    if (**context != '\0') {
        **context = '\0';
        (*context)++; // 移动到下一个字符，准备下一次调用
    }

    return start;
}



// 发送按键按下和松开的函数
NTSTATUS SendKeyPressAndRelease(PDEVICE_CONTEXT deviceContext)
{
    NTSTATUS status;
    HID_XFER_PACKET packet;
    KEYBOARD_INPUT_REPORT pressReport = { 0 };
    KEYBOARD_INPUT_REPORT releaseReport = { 0 };

    // A键的扫描码是0x04
    // 构造按键按下的报告
    pressReport.KeyCodes[0] = 0x04;  // A键按下

    // 发送按下报告
    packet.reportBuffer = (PUCHAR)&pressReport;
    packet.reportBufferLen = sizeof(pressReport);
    packet.reportId = 0;

    status = VhfReadReportSubmit(deviceContext->VhfHandle, &packet);
    if (!NT_SUCCESS(status)) {
        KdPrint(("发送A键按下报告失败: 0x%08x\n", status));
        return status;
    }

    // 短暂延迟，模拟实际按键按下的时间
    LARGE_INTEGER delay;
    delay.QuadPart = -10000 * 5000;  // 50毫秒
    KeDelayExecutionThread(KernelMode, FALSE, &delay);

    // 构造按键松开的报告（所有键码为0表示没有按键按下）
    releaseReport.KeyCodes[0] = 0x00;  // A键松开

    // 发送松开报告
    packet.reportBuffer = (PUCHAR)&releaseReport;

    status = VhfReadReportSubmit(deviceContext->VhfHandle, &packet);
    if (!NT_SUCCESS(status)) {
        KdPrint(("发送A键松开报告失败: 0x%08x\n", status));
        return status;
    }

    KdPrint(("A键按下并松开报告发送成功\n"));
    return STATUS_SUCCESS;
}

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
        // 获取用户模式程序发送给驱动的数据缓冲区
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // 参数1：I/O请求对象
            InputBufferLength,        // 参数2：？？
            &InputBuffer,             // 参数3：输出缓冲区指针的地址
            &InputBufferLengthRet     // 参数4：输出实际获取的缓冲区长度
        );

        //KdPrint(("程序希望驱动返回数据的最大长度是：%Iu\n", OutputBufferLength));//3环程序发送123后OutputBufferLength的值是1024，InputBufferLength是4
        //KdPrint(("应用程序输入数据长度：%Iu\n", InputBufferLength));

        if (!NT_SUCCESS(status)) {
            KdPrint(("获取输入缓冲区失败，状态: 0x%X\n", status));
            break;
        }
        else {
            if (InputBuffer != NULL && InputBufferLengthRet > 0) {
                KdPrint(("输入的ANSI数据内容：%s，长度：%Iu字节\n", InputBuffer, InputBufferLengthRet));
            }
            else {
                KdPrint(("输入缓冲区为空或长度为0\n"));
            }
            //KdPrint(("输入的ANSI数据（字节级）："));
            //for (size_t i = 0; i < InputBufferLengthRet; i++) {
            //    KdPrint(("%02X ", ((PUCHAR)InputBuffer)[i]));
            //}

        }





        //获取驱动需要返回给用户模式程序的数据缓冲区
        status = WdfRequestRetrieveOutputBuffer(
            Request,                  // 参数1：I/O请求对象
            OutputBufferLength,       // 参数2：？？
            &OutputBuffer,            // 参数3：输出缓冲区的指针（内核模式地址）
            &OutputBufferLengthRet    // 参数4：实际可用的输出缓冲区长度
        );



        //复制缓冲区内容
        RtlCopyMemory(OutputBuffer, InputBuffer, InputBufferLength);
        //高数应用程序成功并告知返回消息大小
        WdfRequestSetInformation(Request, InputBufferLengthRet);
        //KdPrint(("返回消息大小是：%Iu 字节\n", InputBufferLengthRet));//应用程序输入123得到4
        //KdPrint(("输入缓冲区：%s\n", InputBuffer));
        //KdPrint(("输出缓冲区：%s\n", OutputBuffer));


        KernelDeleteFile(L"\\??\\C:\\new.txt");

        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request, status);
            return;
        }
        break;
    case IOCTL_Mouse:
        KdPrint(("得到鼠标操作指令"));
        break;
    case IOCTL_Keyboard:
        KdPrint(("得到键盘操作指令"));
        // 获取用户模式程序发送给驱动的数据缓冲区
        status = WdfRequestRetrieveInputBuffer(
            Request,                  // 参数1：I/O请求对象
            InputBufferLength,        // 参数2：？？
            &InputBuffer,             // 参数3：输出缓冲区指针的地址
            &InputBufferLengthRet     // 参数4：输出实际获取的缓冲区长度
        );

        //KdPrint(("程序希望驱动返回数据的最大长度是：%Iu\n", OutputBufferLength));//3环程序发送123后OutputBufferLength的值是1024，InputBufferLength是4
        //KdPrint(("应用程序输入数据长度：%Iu\n", InputBufferLength));

        if (!NT_SUCCESS(status)) {
            KdPrint(("获取输入缓冲区失败，状态: 0x%X\n", status));
            break;
        }


        char* context = NULL;
        char* token;
        char result[10][100] = { 0 };





        //第一次调用，传入字符串和分隔符
        token = custom_strtok(InputBuffer, ',', &context);
        int i = 0;
        while (token != NULL) {
            // 处理提取到的片段
            strcpy(result[i], token);
            i++;
            //KdPrint(("提取到的片段: %s\n", token));
            // 后续调用，第一个参数传NULL
            token = custom_strtok(NULL, ',', &context);
        }
        KdPrint(("提取到的第1个参数: %s\n", result[0]));
        KdPrint(("提取到的第2个参数: %s\n", result[1]));
        KdPrint(("提取到的第3个参数: %s\n", result[2]));
        KdPrint(("提取到的第4个参数: %s\n", result[3]));



        //模拟按下A
        status = SendKeyPressAndRelease(deviceContext);
        if (!NT_SUCCESS(status)) {
            KdPrint(("发送A键操作失败: 0x%08x\n", status));
            return status;
        }

        break;
    case IOCTL_File:
        KdPrint(("得到文件操作指令"));
        break;
    default:
        KdPrint(("未知的控制码"));

    }
    WdfRequestComplete(Request, STATUS_SUCCESS);
}
//&：将变量转换为地址（变量 → 地址）。
//* ：将地址（指针）转换为变量值（地址 → 变量）。
