#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>
#include <ntddk.h>
#include <wdf.h>


// 声明全局变量：用extern，不分配内存，不初始化
extern WDFDEVICE g_Device;


//定义键盘输入报告，不能乱改排序，浪费1整天,我怀疑这个官方的是依次执行而不是判断名字执行，名字可以改
typedef struct _KEYBOARD_INPUT_REPORT
{
    uint8_t ReportId;  // 报告ID
    uint8_t Modifiers;    // 修饰键位图,Shift、Ctrl、Alt 等修饰键的状态(这个可能可以同时按下多个，但是和普通按键输入不同)
    uint8_t Reserved;     // 保留字节
    uint8_t ScanCodes[6]; // 存储同时按下的按键扫描码(支持最多 6 个键的同时按下)
} KEYBOARD_INPUT_REPORT, * PKEYBOARD_INPUT_REPORT;

//定义鼠标输入报告
typedef struct _MOUSE_INPUT_REPORT
{
    uint8_t ReportId;       // 报告ID
    uint8_t Buttons;        // 按键
    char DeltaX;            // x轴
    char DeltaY;            // y轴
    char WheelDelta;        // 滚轮
} MOUSE_INPUT_REPORT, * PMOUSE_INPUT_REPORT;

//定义触摸板输入报告
typedef struct _TOUCHSCREEN_INPUT_REPORT
{
    uint8_t ReportId;           // 报告ID
    uint8_t Buttons;            // 按键状态 (bit0-bit4对应5个按键，bit5-bit7为填充位)
    uint16_t DeltaX;            // X轴绝对位移（0~1023，2字节）
    uint16_t DeltaY;            // Y轴绝对位移（0~1023，2字节）
} TOUCHSCREEN_INPUT_REPORT, * PTOUCHSCREEN_INPUT_REPORT;



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