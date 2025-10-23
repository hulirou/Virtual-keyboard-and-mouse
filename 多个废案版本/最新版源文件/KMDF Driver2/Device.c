#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"


// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);



//���Ӧ����ר�����ɵĹ��ߣ�ĳ�������������̳˵�ģ�
//���̣����ʹ��������豸������
const UCHAR keyboard_mouse_touchScreen_ReportDescriptor[] = {
    // ���̲��� - ����ID=1
    0x05, 0x01,        //   �÷�ҳ��ͨ��������ƣ�
    0x09, 0x06,        //   �÷������̣�
    0xA1, 0x01,        //   ���ϣ�Ӧ�ò㣩
    0x85, 0x01,        //   ����ID��1��
    0x05, 0x07,        //   �÷�ҳ������/С���̣�
    0x19, 0xE0,        //   �÷���Сֵ��0xE0�����μ���ʼ��
    0x29, 0xE7,        //   �÷����ֵ��0xE7�����μ�������
    0x15, 0x00,        //   �߼���Сֵ��0��
    0x25, 0x01,        //   �߼����ֵ��1��
    0x75, 0x01,        //   �����С��1λ��
    0x95, 0x08,        //   ���������8����
    0x81, 0x02,        //   ���루���ݡ�����������ֵ��
    0x95, 0x01,        //   ���������1����
    0x75, 0x08,        //   �����С��8λ��
    0x81, 0x01,        //   ���루������
    0x95, 0x06,        //   ���������6����
    0x75, 0x08,        //   �����С��8λ��
    0x15, 0x00,        //   �߼���Сֵ��0��
    0x25, 0x65,        //   �߼����ֵ��101��
    0x05, 0x07,        //   �÷�ҳ������/С���̣�
    0x19, 0x00,        //   �÷���Сֵ��0x00��
    0x29, 0x65,        //   �÷����ֵ��0x65��
    0x81, 0x00,        //   ���루���ݡ����顢����ֵ��
    0xC0,              //   �������ϣ�Ӧ�ò㣩

    // ��겿�� - ����ID=2
    0x05, 0x01,        //   �÷�ҳ��ͨ��������ƣ�
    0x09, 0x02,        //   �÷�����꣩
    0xA1, 0x01,        //   ���ϣ�Ӧ�ò㣩
    0x85, 0x02,        //   ����ID��2��
    0x09, 0x01,        //   �÷���ָ�룩
    0xA1, 0x00,        //   ���ϣ�����㣩

    // ��갴������
    0x05, 0x09,        //   �÷�ҳ��������
    0x19, 0x01,        //   �÷���Сֵ��0x01��
    0x29, 0x05,        //   �÷����ֵ��0x05��
    0x15, 0x00,        //   �߼���Сֵ��0��
    0x25, 0x01,        //   �߼����ֵ��1��
    0x95, 0x05,        //   ���������5����
    0x75, 0x01,        //   �����С��1λ��
    0x81, 0x02,        //   ���루���ݡ�����������ֵ��

    // ������λ
    0x95, 0x01,        //   ���������1����
    0x75, 0x03,        //   �����С��3λ��
    0x81, 0x01,        //   ���루������

    // ���X��Y���ƶ�
    0x05, 0x01,        //   �÷�ҳ��ͨ��������ƣ�
    0x09, 0x30,        //   �÷���X�ᣩ
    0x09, 0x31,        //   �÷���Y�ᣩ
    0x15, 0x81,        //   �߼���Сֵ��-127��
    0x25, 0x7F,        //   �߼����ֵ��127��
    0x75, 0x08,        //   �����С��8λ��
    0x95, 0x02,        //   ���������2����
    0x81, 0x06,        //   ���루���ݡ����������ֵ��

    // ������
    0x09, 0x38,        //   �÷������֣�
    0x15, 0x81,        //   �߼���Сֵ��-127��
    0x25, 0x7F,        //   �߼����ֵ��127��
    0x75, 0x08,        //   �����С��8λ��
    0x95, 0x01,        //   ���������1����
    0x81, 0x06,        //   ���루���ݡ����������ֵ��

    0xC0,              //   �������ϣ�����㣩
    0xC0,              //   �������ϣ�Ӧ�ò㣩

    // ���������� - ����ID=3
    0x05, 0x01,        //   �÷�ҳ��ͨ�����棩
    0x09, 0x02,        //   �÷�����꣩
    0xA1, 0x01,        //   ���ϣ�Ӧ�ó���
    0x85, 0x03,        //   ����ID��3��
    0x09, 0x01,        //   �÷���ָ�룩
    0xA1, 0x00,        //   ���ϣ�����

    // ��������ť����
    0x05, 0x09,        //   �÷�ҳ����ť��
    0x19, 0x01,        //   �÷���Сֵ��01��
    0x29, 0x05,        //   �÷����ֵ��05��
    0x15, 0x00,        //   �߼���Сֵ��0��
    0x25, 0x01,        //   �߼����ֵ��1��
    0x95, 0x05,        //   ���������5��
    0x75, 0x01,        //   �����С��1��
    0x81, 0x02,        //   ���루���ݣ����������ԣ�

    // ���������λ
    0x95, 0x01,        //   ���������1��
    0x75, 0x03,        //   �����С��3��
    0x81, 0x01,        //   ���루������

    // ��������������X��Y
    0x05, 0x01,        //   �÷�ҳ��ͨ�����棩
    0x09, 0x30,        //   �÷���X���꣩
    0x09, 0x31,        //   �÷���Y���꣩
    0x15, 0x00,        //   �߼���Сֵ��0��
    0x27, 0xFF, 0xFF, 0x00,0x00,//   �߼����ֵ��65535��
    0x75, 0x10,        //   �����С��16��
    0x95, 0x02,        //   ���������2��
    0x81, 0x02,        //   ���루���ݣ����������ԣ�

    0xC0,              //   �������ϣ�����
    0xC0               //   �������ϣ�Ӧ�ó���
};





//����vhf�豸
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

//������
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
    UNICODE_STRING symbolicLinkName;



    //��ʼ���豸��������
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
    //ָ��������
    deviceAttributes.EvtCleanupCallback = EvtDeviceContextCleanup;


    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("VHF-Device: wdf�豸�����豸ʧ�� : 0x%08x\n", status));
        return status;
    }

    //��ȡ�豸�����Ĳ��������г�Ա��ʼ��Ϊ��
    deviceContext = DeviceGetContext(device);
    RtlZeroMemory(deviceContext, sizeof(DEVICE_CONTEXT));



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

    //������������
    RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\huli");
    status = WdfDeviceCreateSymbolicLink(device, &symbolicLinkName);


    // ��ȫ�ֱ���g_Device��ֵ
    g_Device = device;





    //��ʼ��VHF����豸
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(keyboard_mouse_touchScreen_ReportDescriptor), keyboard_mouse_touchScreen_ReportDescriptor);
    //�����豸���ԣ�����ǿ��
    //vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    //vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    //vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    //Ϊ���в�ͬ��ƷID����������豸����
    //vhfConfig.VendorID = 0x045E;  //Microsoft��Ӧ��ID
    //vhfConfig.ProductID = 0x0040; //����ƷID������̲�ͬ��
    //vhfConfig.VersionNumber = 0x0001;

    //����vhf����豸
    status = VhfCreate(&vhfConfig, &deviceContext->vhfHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf����豸����ʧ�� 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf����豸�����ɹ�\n"));

    //����vhf����豸
    status = VhfStart(deviceContext->vhfHandle);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("vhf����豸����ʧ�� 0x%08x\n", status));
        return status;
    }
    KdPrint(("vhf����豸�����ɹ�\n"));




}