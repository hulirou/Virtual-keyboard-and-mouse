import ctypes

huli_geampad = ctypes.CDLL("./huli_geampad.dll")

# 获取设备句柄，我们的设备输入报告交给这个设备
hDevice = huli_geampad.getHandleDevice()


# 以防万一用来终止输入的
huli_geampad.sendKeyboardReportToDriver(hDevice, 0x00, 0x00)





