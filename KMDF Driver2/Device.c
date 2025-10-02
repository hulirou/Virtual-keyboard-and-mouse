#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <vhf.h>
#include "Device.h"


// {685186F1-995E-40E4-BDD4-184723D15E94}
DEFINE_GUID(DEVICEINTERFACE,
    0x685186f1, 0x995e, 0x40e4, 0xbd, 0xd4, 0x18, 0x47, 0x23, 0xd1, 0x5e, 0x94);



//���Ӧ����ר�����ɵĹ��ߣ�ĳ�������������̳˵�ģ�
//���̵��豸������
const UCHAR keyboardReportDescriptor[] = {
    0x05, 0x01,        //   �÷�ҳ��ͨ��������ƣ�
    0x09, 0x06,        //   �÷������̣�
    0xA1, 0x01,        //   ���ϣ�Ӧ�ò㣬��ʶ��Ϊ�����豸�Ĺ��ܼ��ϣ�
    0x05, 0x07,        //   �÷�ҳ������/С���̣�
    0x19, 0xE0,        //   �÷���Сֵ��0xE0����Ӧ��Ctrl�����μ���ʼ��
    0x29, 0xE7,        //   �÷����ֵ��0xE7����Ӧ��GUI�����μ�������
    0x15, 0x00,        //   �߼���Сֵ��0��������δ���£�
    0x25, 0x01,        //   �߼����ֵ��1�����������£�
    0x75, 0x01,        //   �����С��1λ��ÿ�����μ���1λ��ʾ״̬��
    0x95, 0x08,        //   ���������8������8�����μ���
    0x81, 0x02,        //   ���루���ݡ�����������ֵ�����μ�״̬�������ֶΣ�
    0x95, 0x01,        //   ���������1����
    0x75, 0x08,        //   �����С��8λ��
    0x81, 0x01,        //   ���루���ݡ����顢����ֵ�����λ�ֶΣ���ʵ�ʹ��ܣ�
    0x95, 0x06,        //   ���������6����֧��ͬʱ����6����ͨ������
    0x75, 0x08,        //   �����С��8λ��ÿ��������1�ֽ�ɨ�����ʾ��
    0x15, 0x00,        //   �߼���Сֵ��0���ް�������ʱ��ɨ���룩
    0x25, 0x65,        //   �߼����ֵ��101����Ӧ���������Чɨ���룩
    0x05, 0x07,        //   �÷�ҳ������/С���̣�
    0x19, 0x00,        //   �÷���Сֵ��0x00���ް���״̬��
    0x29, 0x65,        //   �÷����ֵ��0x65�������Ч����ɨ���룩
    0x81, 0x00,        //   ���루���ݡ����顢����ֵ����ͨ����ɨ����������ֶΣ�
    0xC0,              //   �������ϣ�����Ӧ�ò㼯�ϣ�
};

//�����豸������
const UCHAR mouseReportDescriptor[] = {
    0x05, 0x01,        //   �÷�ҳ��ͨ��������ƣ�
    0x09, 0x02,        //   �÷�����꣩
    0xA1, 0x01,        //   ���ϣ�Ӧ�ò㣬��ʶ��Ϊ����豸�Ĺ��ܼ��ϣ�
    0x09, 0x01,        //   �÷���ָ�룬���ĺ��Ĺ������ͣ�
    0xA1, 0x00,        //   ���ϣ�����㣬������������ʵ�壩
    0x05, 0x09,        //   �÷�ҳ��������
    0x19, 0x01,        //   �÷���Сֵ��0x01����Ӧ��������
    0x29, 0x03,        //   �÷����ֵ��0x03����Ӧ����м���
    0x15, 0x00,        //   �߼���Сֵ��0������δ���£�
    0x25, 0x01,        //   �߼����ֵ��1���������£�
    0x95, 0x03,        //   ���������3����֧�����ҡ���3��������
    0x75, 0x01,        //   �����С��1λ��ÿ��������1λ��ʾ״̬��
    0x81, 0x02,        //   ���루���ݡ�����������ֵ������״̬�������ֶΣ�
    0x95, 0x01,        //   ���������1����
    0x75, 0x05,        //   �����С��5λ��
    0x81, 0x01,        //   ���루���ݡ����顢����ֵ�����λ�ֶΣ�����1�ֽڣ�
    0x05, 0x01,        //   �÷�ҳ��ͨ��������ƣ�
    0x09, 0x30,        //   �÷���X�ᣬ���ˮƽ�����ƶ���
    0x09, 0x31,        //   �÷���Y�ᣬ��괹ֱ�����ƶ���
    0x15, 0x81,        //   �߼���Сֵ��-127������������ƶ�����
    0x25, 0x7F,        //   �߼����ֵ��127������������ƶ�����
    0x75, 0x08,        //   �����С��8λ��ÿ�����ƶ�����1�ֽڱ�ʾ��
    0x95, 0x02,        //   ���������2������ӦX��Y�����ᣩ
    0x81, 0x06,        //   ���루���ݡ����������ֵ��X/Y���ƶ����������ֶΣ�
    0x09, 0x38,        //   �÷������֣�
    0x15, 0x81,        //   �߼���Сֵ��-127��������������������
    0x25, 0x7F,        //   �߼����ֵ��127��������������������
    0x75, 0x08,        //   �����С��8λ�������ƶ�����1�ֽڱ�ʾ��
    0x95, 0x01,        //   ���������1������Ӧ���֣�
    0x81, 0x06,        //   ���루���ݡ����������ֵ�������ƶ����������ֶΣ�
    0xC0,              //   �������ϣ���������㼯�ϣ�
    0xC0,              //   �������ϣ�����Ӧ�ò㼯�ϣ�
};




//����vhf�豸
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
    VHF_CONFIG_INIT(&vhfConfig, WdfDeviceWdmGetDeviceObject(device), sizeof(mouseReportDescriptor), mouseReportDescriptor);
    //�����豸���ԣ�����ǿ��
    //vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    //vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    //vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    //Ϊ���в�ͬ��ƷID����������豸����
    //vhfConfig.VendorID = 0x045E;  //Microsoft��Ӧ��ID
    //vhfConfig.ProductID = 0x0040; //����ƷID������̲�ͬ��
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




}
