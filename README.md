



键盘数据的完整路径（从硬件到用户层）：

        硬件（键盘） → 内核层驱动 → 内核输入子系统 → 用户态驱动（可选） → 用户层 API → 应用程序
                          ⬆             ⬆                                  ⬆
                  我的虚拟键鼠位置  我的监听器位置                    常见的键鼠模拟位置
    









文件介绍：
driver.c是主要文件，使用EvtDriverDeviceAdd后会被动执行到Device.c的EvtDriverDeviceAdd函数
Device.c是创建各种设备的代码，如：wdf设备、vhf设备
Queeu.c是用于处理IO请求，即三环程序和应用程序的通信。我在键盘控制码的通信中添加了提交HID输入报告，模拟了一次长按A键5秒。由于这个方法需要设备上下文，这个设备上下文在Device.c文件中，我使用了全局设备句柄，在Device.c完成工作后把句柄赋值给了全局句柄，让Queeu.c
tool.c是用户监听设备输入，hook了kbdclass.sys驱动中的KeyboardClassServiceCallback函数，然后把收到的键盘数据放入一个128大小的队列中供dll程序读取，python使用循环读取队列中的键盘数据，做到判断快捷键是否按下，用于快速完成yywj游戏的连招




安装方法：
需要电脑禁用驱动签名校验，在恢复->高级启动->疑难解答->高级选项->启动设置->F7
或者使用如下bat命令快速进入高级启动：
@echo off
shutdown /r /o /f /t 0
成功进入测试模式后，在管理员模式的cmd中输入devcon.exe install "KMDFDriver2.inf" root\KMDFDriver2
各种文件路径要正确，在生成的解决方案中要有x64的devcon.exe（这个东西WDK包自带的文件夹有，我会在该git中附带）

我添加了清理vhf设备的函数，现在应该不会有什么明显的错误需要重启了



逻辑分析：
创建一个wdf设备后创建vhf复合型子设备和在输入子系统中添加一个钩子把内容跳转到监听函数后再转回去继续执行，发送键鼠数据/监听键盘依靠vhf字设备。


参考：
inf文件需要写什么类型等：https://github.com/SenuthLikesCrak/Virtual-HID-Framework-gamepad-example

微软VHF官方用法：https://learn.microsoft.com/zh-cn/windows-hardware/drivers/hid/virtual-hid-framework--vhf-

WDF驱动框架基础：https://www.bilibili.com/video/BV1cu411R7HY/?spm_id_from=333.337.search-card.all.click&vd_source=4f2ab0a07d42ba5c0b331344c619e3b0



想说的：
最初只是我想用于刷游戏副本
基本上写完了，现在学习下c语言如何写一个动态链接库dll，这样的话我的python也能运行了，我确实不习惯使用c编写这些代码。
还在写ing...



该代码/程序将永久免费使用
直接禁止将代码（或衍生作品）用于商业盈利，例如：
禁止将代码作为商业产品销售（如闭源软件、付费 API 服务）；
允许个人使用、修改和非商业分发，但商业场景需额外购买授权。
我不是很懂apache2.0等协议，所以这里的声明权重大于协议，其他的均按协议规则使用


因为我花了很多时间没有收费，所以如果你们直接使用我的代码去收费我会感到不适

如有解释不周，请见谅。本人零基础
