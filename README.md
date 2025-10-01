零基础，勿喷

文件介绍：
driver.c是主要文件，使用EvtDriverDeviceAdd后会被动执行到Device.c的EvtDriverDeviceAdd函数
Device.c是创建各种设备的代码，如：wdf设备、vhf设备
Queeu.c是用于处理IO请求，即三环程序和应用程序的通信。我在键盘控制码的通信中添加了提交HID输入报告，模拟了一次长按A键5秒。由于这个方法需要设备上下文，这个设备上下文在Device.c文件中，我使用了全局设备句柄，在Device.c完成工作后把句柄赋值给了全局句柄，让Queeu.c通过全局句柄获取到了设备上下文。（设备上下文需要设备句柄）


安装方法：
需要电脑禁用驱动签名校验，在恢复->高级启动->疑难解答->高级选项->启动设置->F7
或者使用如下bat命令快速进入高级启动：
@echo off
shutdown /r /o /f /t 0
在cmd命令中输入devcon.exe install "KMDFDriver2.inf" root\KMDFDriver2
各种文件路径要正确，在生成的解决方案中要有x64的devcon.exe（这个东西WDK包自带的文件夹有）

我添加了清理vhf设备的函数，现在应该不会有什么明显的错误需要重启了


3环程序使用说明：
双击使用，根据提示输入数字。dll动态链接库还在写...


逻辑分析：
创建一个wdf设备后创建vhf键盘设备和vhf鼠标设备，wdf设备句柄可以获取设备上下文，设备上下文中存储键盘和鼠标的句柄，发送键鼠数据依靠这两个设备句柄。安装时会出现你的wdf设备，重启电脑后显示另外两个设备，他们控制键盘和鼠标。


参考：
inf文件需要写什么类型等：https://github.com/SenuthLikesCrak/Virtual-HID-Framework-gamepad-example

微软VHF官方用法：https://learn.microsoft.com/zh-cn/windows-hardware/drivers/hid/virtual-hid-framework--vhf-

WDF驱动框架基础：https://www.bilibili.com/video/BV1cu411R7HY/?spm_id_from=333.337.search-card.all.click&vd_source=4f2ab0a07d42ba5c0b331344c619e3b0




想说的：
基本上写完了，现在学习下c语言如何写一个动态链接库dll，这样的话我的python也能运行了，我确实不习惯使用c编写这些代码。
还在写ing...

