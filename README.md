可编程的LED
===============
可编程的LED是一个小巧的电路板，有两颗全彩LED灯。它提供了一个Mmacro USB接口，可以连接到电脑，通过简单编程控制LED灯的闪烁。

设计目标
--------
* 体积2.8cm * 5.5cm(PCB尺寸)。
* 设计使用两节5号电池。
* 通过USB接口对灯进行编程（设备会被识别成U盘，无需驱动）。
* 软硬件设计开源。

应用
-------
* 自行车灯，提高车辆夜间识别度，增强安全性。
* 冒充汽车防盗器，吓走不怀好意者。
* 玩具。

硬件资源
--------
* STM32f103c8t处理器
* 两个全彩LED灯
* 一个macro USB接口
* 一个三线串口
* 一个SWD接口（包括SWO）
* 五个GPIO
* 震动开关

当前进度
--------
第一版PCB制作完成。

开发博客
--------
<http://blog.sina.com.cn/lgeek>

Programmable LED
===============
ProgrammableLED is a small circuit board with two RGB LEDs. It provide a macro USB port, we can connect it to computer to programme LED flash pattern.

Design feature
--------------
* size 2.8cm * 5.5cm(PCB size).
* use 2 AA batteries.
* programming through macro USB port(device will be recognized as USB massive storage device).
* open source on both hardware and software.

Application
-----------
* Bicycle light, improve identification at night.
* Fake car alarm, scare away bad guys.
* Toys.

Hardware
--------
* STM32f103c8t MCU
* 2 RGB LED
* 1 macro USB port
* 1 usart port
* 1 SWD(including SWO)
* 5 GPIO
* Vibration switch

Current progress
---------------
Finished first hardware(first version).

Development blog
---------------
<http://lsgeek.blogspot.com>

