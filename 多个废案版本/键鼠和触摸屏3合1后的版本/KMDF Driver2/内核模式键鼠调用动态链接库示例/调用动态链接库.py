import ctypes
import time


# 加载DLL
huli_geampad = ctypes.CDLL("./huli_geampad.dll")  # 注意路径正确性

# 获取设备句柄，我们的设备输入报告交给这个设备
hDevice = huli_geampad.getHandleDevice()

# 定义一些输入参数的类型，让后面的操作更轻松
huli_geampad.keyboardPressAndRelease.argtypes = [
    ctypes.c_void_p,  # 对应C++的 HANDLE（本质是void*）
    ctypes.c_char_p,  # 对应C++的 const char*（自动将Python str转为字节串）
    ctypes.c_char_p,  # 对应C++的 const char*
    ctypes.c_int      # 对应C++的 int
]

time.sleep(1)
# 按下并释放，参数：设备句柄，按键扫描码，修饰符的扫描码，按下后多久释放（毫秒）
huli_geampad.keyboardPressAndRelease(hDevice, b"a", b"Shift", 1000)

# 发送键盘输入到驱动，参数：设备句柄，按键扫描码，修饰符扫描码(危险的使用)
# huli_geampad.sendKeyboardReportToDriver(hDevice, 0x04, 0x00)

# 发送鼠标输入到驱动，参数：设备句柄，按键扫描码，x轴移动，y轴移动，滚轮滚动（值应该是-127~127）,x正向右，y正向下，滚轮正向上翻页.
# 如果没有弹起，huli_geampad再次使用会不灵
# huli_geampad.sendMouseReportToDriver(hDevice, "Null", 0, 0, 0)


# 想要连续键盘输入就自己写吧，我的代码是不区分大小写的，写个字符串遍历循环使用写好的方法输入就行了


