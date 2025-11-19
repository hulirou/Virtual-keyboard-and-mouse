#include "tool.h"
#include <ntimage.h>
#include <windef.h>//是FindPatternImage相关的库

// 自定义结构体，多个系统模块的的各种信息
PSYSTEM_MODULE_INFORMATION pSystemModuleTable = NULL;
// 键盘类服务回调函数指针（原始函数）
fnKeyboardClassServiceCallback pKeyboardClassServiceCallback = NULL;
// 键盘类服务回调函数指针（桥接函数，用于钩子跳转）
fnKeyboardClassServiceCallback hkBridgeKeyboardClassServiceCallback = NULL;
// 保存原始指令的缓冲区（用于钩子卸载时恢复）
unsigned char OrigOpcodes[12];









//获取系统模块信息
PSYSTEM_MODULE_INFORMATION GetSystemModuleInformation() {
    // 确保在合适的IRQL级别（<= DISPATCH_LEVEL）
    if (KeGetCurrentIrql() <= DISPATCH_LEVEL) {
        ULONG szModule = 0;
        // 先查询所需缓冲区大小
        NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, 0, &szModule);
        if (STATUS_INFO_LENGTH_MISMATCH != status) {
            DbgPrint("ZwQuerySystemInformation获取大小失败: %p !\n", status);
            return NULL;
        }
        DbgPrint("ZwQuerySystemInformation所需大小: %d\n", szModule);

        // 分配缓冲区
        PSYSTEM_MODULE_INFORMATION pBuffer = ExAllocatePool(NonPagedPool, szModule);
        if (!pBuffer) {
            DbgPrint("为模块信息分配%d字节内存失败 !\n", szModule);
            return NULL;
        }

        // 再次调用获取实际模块信息
        if (!NT_SUCCESS(status = ZwQuerySystemInformation(SystemModuleInformation, pBuffer, szModule, 0))) {
            ExFreePool(pBuffer);  // 失败时释放内存
            DbgPrint("ZwQuerySystemInformation获取信息失败: %p !\n", status);
            return NULL;
        }
        return pBuffer;
    }
    return NULL;
}

// 遍历输出系统模块的数量和名称
void PrintSystemModules() {
    if (pSystemModuleTable == NULL) {
        DbgPrint("系统模块表未初始化！\n");
        return;
    }

    // 输出模块总数
    DbgPrint("系统模块总数: %u\n", pSystemModuleTable->NumberOfModules);

    // 遍历所有模块，输出名称
    for (ULONG i = 0; i < pSystemModuleTable->NumberOfModules; i++) {
        // 获取当前模块信息
        PSYSTEM_MODULE pModule = &pSystemModuleTable->Modules[i];
        // 从FullPathName中提取文件名（通过OffsetToFileName偏移）
        PCHAR fileName = (PCHAR)pModule->FullPathName + pModule->OffsetToFileName;
        // 输出模块索引、文件名
        DbgPrint("模块 %u: %s\n", i, fileName);
    }
}

//初始化系统模块表
NTSTATUS InitializeSystemModuleTable() {
    //如果有旧表就释放
    if (pSystemModuleTable) {
        ExFreePool(pSystemModuleTable);
        pSystemModuleTable = NULL;
    }
    // 首次初始化
    pSystemModuleTable = GetSystemModuleInformation();






    // 检查模块表是否初始化成功
    if (pSystemModuleTable == NULL) {
        DbgPrint("系统模块表未初始化或初始化失败！\n");
        return;
    }

    //顺手便利出所有驱动
    PrintSystemModules();

    //非空即成功
    return pSystemModuleTable ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

//释放系统模块表的内存
NTSTATUS FreeSystemModuleTable() {
    if (pSystemModuleTable) {
        ExFreePool(pSystemModuleTable);
        pSystemModuleTable = NULL;
    }
    return STATUS_SUCCESS;
}






//大写转小写
PCHAR LowerStr(PCHAR str) {
    for (PCHAR s = str; *s; ++s) {
        *s = (CHAR)tolower(*s);
    }
    return str;
}

//根据模块名称找到对应的地址
PVOID GetBaseAddress(IN PCHAR pModuleName, OUT PULONG pSize) {
    PVOID pModuleBase = NULL;
    PSYSTEM_MODULE_INFORMATION pBuffer = GetSystemModuleInformation();
    if (!pBuffer) {
        DbgPrint("获取系统模块信息失败。\n");
        return pModuleBase;
    }

    // 遍历所有模块查找匹配项
    for (int i = 0; i < pBuffer->NumberOfModules; i++) {
        // 将模块路径转为小写后检查是否包含目标模块名
        if (strstr(LowerStr((PCHAR)pBuffer->Modules[i].FullPathName), pModuleName)) {
            pModuleBase = pBuffer->Modules[i].ImageBase;
            if (pSize) {
                *pSize = pBuffer->Modules[i].ImageSize;
            }
            break;
        }
    }
    ExFreePool(pBuffer);  // 释放模块信息缓冲区
    return pModuleBase;
}

//用于验证内存中的一段字节是否与给定的 “特征码（pattern）” 和 “掩码（mask）” 匹配
BOOL CheckMask(PCHAR base, PCHAR pattern, PCHAR mask) {
    for (; *mask; ++base, ++pattern, ++mask) {
        // 掩码为'x'时必须精确匹配
        if ('x' == *mask && *base != *pattern) {
            return FALSE;
        }
    }
    return TRUE;
}

//内存中搜索特定的字节
PVOID FindPattern(PCHAR base, ULONG length, PCHAR pattern, PCHAR mask) {
    // 计算最大查找范围（减去模式长度）
    length -= (DWORD)strlen(mask);
    for (DWORD i = 0; i <= length; ++i) {
        PVOID addr = &base[i];
        if (CheckMask(addr, pattern, mask)) {
            return addr;
        }
    }
    return 0;
}

//在 PE 镜像（可执行文件或动态链接库等）中查找特定字节（我不理解）
PVOID FindPatternImage(PCHAR base, PCHAR pattern, PCHAR mask) {
    PVOID match = 0;
    // 获取PE文件头
    PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)(base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);
    // 获取节表
    PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION(headers);

    // 遍历所有节
    for (DWORD i = 0; i < headers->FileHeader.NumberOfSections; ++i) {
        PIMAGE_SECTION_HEADER section = &sections[i];
        // 检查是否为代码节（.text或节名反转的TEXT）
        if ('EGAP' == *(PINT)section->Name || memcmp(section->Name, ".text", 5) == 0) {
            // 在代码节中查找模式
            match = FindPattern(base + section->VirtualAddress, section->Misc.VirtualSize, pattern, mask);
            if (match) {
                break;
            }
        }
    }
    return match;
}

//查找KeyboardClassServiceCallback函数地址
NTSTATUS getKeyboardClassServiceCallback(PVOID moduleBase, PVOID* pOutAddr) {
    if (!moduleBase) {  // 模块基地址无效
        return STATUS_UNSUCCESSFUL;
    }
    // 在模块中搜索特征码（用于定位函数引用）
    PVOID pFunctionRef = FindPatternImage(moduleBase, "\xB9\x03\x02\x0B\x00\x48\x8D\x05", "xxxxxxxx"); // 适配19045.2251版本
    if (pFunctionRef) {
        // 计算实际函数地址（根据特征码偏移和相对地址）
        PVOID pKeyboardClassServiceCallback = (ULONG64)pFunctionRef +
            0x5/*前置指令长度*/ +
            0x7/*当前指令长度*/ +
            *(INT32*)((ULONG64)pFunctionRef + 0x5 + 0x3);
        *pOutAddr = pKeyboardClassServiceCallback;
        return STATUS_SUCCESS;
    }
    return STATUS_UNSUCCESSFUL;
}

//系统加载的模块≈驱动（内核态） + DLL（用户态） + 运行的 EXE 主模块
NTSTATUS GetModuleFullPathNameByRegion(IN PSYSTEM_MODULE_INFORMATION pSystemModuleInformations, IN PVOID Address, OUT CHAR* oBuffer) {
    if (!pSystemModuleInformations || !Address) {
        return STATUS_UNSUCCESSFUL;
    }

    // 遍历模块查找包含该地址的模块
    for (int i = 0; i < pSystemModuleInformations->NumberOfModules; i++) {
        // 检查地址是否在当前模块的地址范围内
        if (Address > (ULONG64)pSystemModuleInformations->Modules[i].ImageBase &&
            Address < (ULONG64)pSystemModuleInformations->Modules[i].ImageBase + pSystemModuleInformations->Modules[i].ImageSize) {
            strcpy(oBuffer, pSystemModuleInformations->Modules[i].FullPathName);
            return STATUS_SUCCESS;
        }
    }
    return STATUS_UNSUCCESSFUL;
}

//让hook的函数跳转到该函数后跳转回去
void __fastcall hkKeyboardClassServiceCallback(PDEVICE_OBJECT DeviceObject, PKEYBOARD_INPUT_DATA InputDataStart, PKEYBOARD_INPUT_DATA InputDataEnd, PULONG InputDataConsumed) {

    CHAR buffer[256];  // 用于存储模块路径的缓冲区
    // 获取调用者模块的完整路径
    NTSTATUS status = GetModuleFullPathNameByRegion(pSystemModuleTable, _ReturnAddress(), buffer);

    // 遍历所有输入数据
    for (int i = 0; i < (InputDataEnd - InputDataStart); i++) {
        if (NT_SUCCESS(status)) {
            // 输出调用者路径、扫描码和按键状态（按下/抬起）
            DbgPrint("调用者：%s  扫描码：%d，按键状态：%s\n",
                buffer,
                (InputDataStart + i)->MakeCode,  // 修正指针偏移（原代码偏移计算有误，此处按逻辑修正）
                (InputDataStart + i)->Flags ? "抬起" : "按下");
        }
        else {
            // 若无法获取模块路径，输出返回地址、扫描码和按键状态
            DbgPrint("返回地址：%p  扫描码：%d，按键状态：%s\n",
                _ReturnAddress(),
                (InputDataStart + i)->MakeCode,  // 修正指针偏移
                (InputDataStart + i)->Flags ? "抬起" : "按下");
        }
    }

    // 调用桥接的原始回调函数，保证读取后能函数可以继续完成
    return hkBridgeKeyboardClassServiceCallback(DeviceObject, InputDataStart, InputDataEnd, InputDataConsumed);
}


//安装键盘钩子
NTSTATUS installKeyboardHook() {
    // 若已安装钩子，直接返回成功
    if (hkBridgeKeyboardClassServiceCallback)
        return STATUS_SUCCESS;

    NTSTATUS status = STATUS_SUCCESS;
    ULONG szModule = 0;  // 模块大小变量

    // 获取kbdclass.sys的基地址
    PVOID kbdclass = GetBaseAddress("kbdclass.sys", &szModule);
    if (!kbdclass) {
        DbgPrint("找不到kbdclass.sys。\n");
        return STATUS_UNSUCCESSFUL;
    }

    // 获取KeyboardClassServiceCallback函数地址
    status = getKeyboardClassServiceCallback(kbdclass, &pKeyboardClassServiceCallback);
    if (status != STATUS_SUCCESS) {
        DbgPrint("找不到KeyboardClassServiceCallback函数。\n");
        return STATUS_UNSUCCESSFUL;
    }

    // 验证函数地址有效性
    if (!MmIsAddressValid(pKeyboardClassServiceCallback)) {
        DbgPrint("验证KeyboardClassServiceCallback地址失败。\n");
        return STATUS_UNSUCCESSFUL;
    }

    DbgPrint("已找到KeyboardClassServiceCallback：%p。\n", pKeyboardClassServiceCallback);

    // 获取函数的物理地址（用于内存映射修改）
    PHYSICAL_ADDRESS PAKeyboardClassServiceCallback = MmGetPhysicalAddress(pKeyboardClassServiceCallback);


    // 如果物理地址有效
    if (PAKeyboardClassServiceCallback.QuadPart)
    {
        // 将物理地址映射到虚拟地址（大小1024字节）
        PVOID VAKeyboardClassServiceCallback = MmMapIoSpaceEx(PAKeyboardClassServiceCallback, 1024, 4i64);
        if (VAKeyboardClassServiceCallback)
        {
            BOOLEAN bFound = FALSE;  // 标记是否找到函数起始指令
            if (!hkBridgeKeyboardClassServiceCallback) {
                int i;
                // 搜索函数起始的0x55指令（push rbp，常见函数开头）
                for (i = 0; i < 64; i++) {
                    unsigned char Opcode = *(unsigned char*)((ULONG64)VAKeyboardClassServiceCallback + i);
                    if (Opcode == 0x55) {
                        bFound = TRUE;
                        break;
                    }
                }
                if (bFound) {
                    // 获取系统模块信息（可能用于后续地址解析）
                    GetSystemModuleInformation();

                    // 构造跳转指令：mov rax, 目标地址; jmp rax
                    unsigned char JumpOrig[] = { 0x48, 0xB8, 0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  // mov rax, 原始函数偏移地址
                                            0xFF, 0xE0 /* jmp rax */ };

                    // 分配非分页内存作为桥接函数（保存原始指令+跳转）
                    hkBridgeKeyboardClassServiceCallback = ExAllocatePool(NonPagedPool, 1024);
                    // 复制原始函数的前i字节指令到桥接函数
                    RtlCopyMemory(hkBridgeKeyboardClassServiceCallback, (PVOID)pKeyboardClassServiceCallback, i);
                    // 设置跳转目标为原始函数的i字节偏移处（跳过被覆盖的指令）
                    *(PVOID*)(JumpOrig + 2) = (ULONG64)pKeyboardClassServiceCallback + i;
                    // 将跳转指令复制到桥接函数的i字节偏移处
                    RtlCopyMemory((ULONG64)hkBridgeKeyboardClassServiceCallback + i, JumpOrig, sizeof(JumpOrig));

                    {
                        // 构造钩子跳转指令：跳转到我们的钩子函数
                        unsigned char BT[] = { 0x48, 0xB8, 0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  // mov rax, 钩子函数地址
                                        0xFF, 0xE0 /* jmp rax */ };
                        // 设置跳转目标为钩子回调函数
                        *(PVOID*)(BT + 2) = hkKeyboardClassServiceCallback;
                        // 保存原始指令（用于卸载时恢复）
                        RtlCopyMemory(OrigOpcodes, VAKeyboardClassServiceCallback, sizeof(BT));
                        // 将钩子跳转指令写入原始函数地址（覆盖原始指令）
                        RtlCopyMemory(VAKeyboardClassServiceCallback, BT, sizeof(BT));
                        // 解除内存映射
                        MmUnmapIoSpace(VAKeyboardClassServiceCallback, 1024);
                        status = STATUS_SUCCESS;
                        return status;
                    }
                }
                else {
                    DbgPrint("安装钩子失败[0]\n");
                }
            }
        }
    }
    return STATUS_UNSUCCESSFUL;
}

//卸载键盘钩子
NTSTATUS uninstallKeyboardHook() {
    // 若钩子已安装（存在原始函数和桥接函数）
    if (pKeyboardClassServiceCallback && hkBridgeKeyboardClassServiceCallback) {
        // 获取原始函数的物理地址
        PHYSICAL_ADDRESS PAKeyboardClassServiceCallback = MmGetPhysicalAddress(pKeyboardClassServiceCallback);
        if (PAKeyboardClassServiceCallback.QuadPart)  // 物理地址有效
        {
            // 映射物理地址到虚拟地址
            PVOID VAKeyboardClassServiceCallback = MmMapIoSpaceEx(PAKeyboardClassServiceCallback, 1024, 4i64);
            if (VAKeyboardClassServiceCallback) {
                // 恢复原始指令（卸载钩子）
                RtlCopyMemory(VAKeyboardClassServiceCallback, OrigOpcodes, sizeof(OrigOpcodes));
                // 解除内存映射
                MmUnmapIoSpace(VAKeyboardClassServiceCallback, 1024);
                // 释放桥接函数内存
                ExFreePool(hkBridgeKeyboardClassServiceCallback);
                hkBridgeKeyboardClassServiceCallback = NULL;
                return STATUS_SUCCESS;
            }
        }
    }
    return STATUS_UNSUCCESSFUL;
}



