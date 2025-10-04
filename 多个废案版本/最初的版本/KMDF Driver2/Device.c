#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"


// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);


//���̵��豸������
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

//�����豸������
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


// ����ȫ�ֱ�������һ�Σ������ڴ棬�ɳ�ʼ��
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
        KdPrint(("VHF-Device: wdf�豸�����豸ʧ�� : 0x%08x\n", status));
        return status;
    }

    deviceContext = DeviceGetContext(device);
    RtlZeroMemory(deviceContext, sizeof(DEVICE_CONTEXT));

    //��ʼ��VHF����
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(keyboardReportDescriptor), keyboardReportDescriptor);
    //����vhf�����豸���ڶ�������ָ�����Զ�����豸�����Ľṹ�������ṩһ����ʼ���õ������ļ�����Ҫvhf�豸��Ҫ��ŵľ��λ�ã�
    //����߼����豸������豸�����ģ��豸�����İ������̺�����vhf�豸���
    status = VhfCreate(&vhfConfig, &deviceContext->vhfKeyboardHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf�����豸����ʧ�� 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf�����豸�����ɹ�\n"));

    //����vhf�����豸
    status = VhfStart(deviceContext->vhfKeyboardHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf�����豸����ʧ�� 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf�����豸�����ɹ�\n"));






    //��ʼ��VHF���
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(keyboardReportDescriptor), mouseReportDescriptor);
    //�����豸���ԣ�����ǿ��
    //vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    //vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    //vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    //// Set device attributes for mouse with different product ID
    //vhfConfig.VendorID = 0x045E;  // Microsoft vendor ID
    //vhfConfig.ProductID = 0x0040; // Mouse product ID (different from keyboard)
    //vhfConfig.VersionNumber = 0x0001;

    //����vhf����豸
    status = VhfCreate(&vhfConfig, &deviceContext->vhfMouseHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf����豸����ʧ�� 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf����豸�����ɹ�\n"));

    //����vhf����豸
    status = VhfStart(deviceContext->vhfMouseHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf����豸����ʧ�� 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf����豸�����ɹ�\n"));




    //����IO���к���
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&IoConfig, WdfIoQueueDispatchSequential);
    IoConfig.EvtIoDeviceControl = EvtIoDeviceControl;
    status = WdfIoQueueCreate(device, &IoConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    if (!NT_SUCCESS(status)) {
        KdPrint(("IO���д���ʧ�� 0x%08X\n", status));
    }
    else {
        KdPrint(("IO���д������ 0x%08X\n", status));
    }

    //�����豸�ӿ�
    WdfDeviceCreateDeviceInterface(device, &DEVICEINTERFACE, NULL);

    // ��ȫ�ֱ���g_Device��ֵ
    g_Device = device;
}



