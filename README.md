自动刹车灯
===============
自动刹车灯是一个小巧的电路板，在刹车减速时自动亮起。可以将其安装在自行车上，用于警示其他车辆和行人。自动刹车灯由电池供电并内置加速度传感器，因此无需额外连接其他线缆。使用两节5号电池时，设计待机时间为一年以上(待机功耗66微安)，基本可以实现永不关机，即装即忘。项目命名为“ProgrammableLED”是最初的初衷，现在已经简化为自动刹车等了，功能简化体验更优。
![image](https://raw.github.com/lishen2/ProgrammableLED/master/images/finished2.JPG)

特性
--------
* 体积2.8cm * 5.5cm(PCB尺寸)。
* 自动识别减速刹车。
* 停车自动休眠，设计待机时间一年以上。
* 软硬件设计开源。

应用
-------
* 自行车灯，提高车辆夜间识别度，增强安全性。

硬件资源
--------
* STM32F103C8T处理器
* 两个全彩LED灯，两个红色LED（1206）
* 加速度传感器ADXL345
* 一个三线串口
* 一个三线SWD接口

当前进度
--------
20140217: 第一版PCB制作完成.                     
20140220: 编写代码，STM32的USB驱动示例代码很烂，作为程序员不忍直视。              
20140319: 放弃使用USB口，原因是USB驱动比较复杂一直没有调试成功。                  
20140329: 第二版PCB制作完成，放弃USB接口，增加加速度传感器ADXL234和升压电路（保证LED亮度一致）                    
20140410：还在编写软件，放弃编程功能，仅留下刹车灯功能。                   
20140506：基本功能正常，正在优化功耗。
20140515：耗降到66微安，正在调整对加速度数值的处理。
20140601：加速度传感器参数调整完毕，经过几周实测，效果还不错。

开发博客
--------
<http://blog.sina.com.cn/lgeek>

Automatic Break Light
===============
Automatic Break Light is a small circuit board, it can automatically light up when decelerate. It can installed on a bicycle to caution other bicycle,  vehicle and people. Automatic Break Light has an accelerate sensor on board, so it will work without additional sensor or wire. It use two AA battery as power supply, designed to work at least 12 month(Standby power consumption is 66 uA). You no longer need to manually shut it down, and you can forget it after install. The project name "ProgrammableLED" is the original idea, but now it has enolved into "Automatic Break Light". Fewer functions, better experience.

Design feature
--------------
* size 2.8cm * 5.5cm(PCB size).
* recognize decelerate.
* automatic sleep when stop, standby lifetime more than 12 month.
* open source on both software and hardware.

Application
-----------
* Bicycle light, improve identification at night.

Hardware
--------
* STM32F103C8T MCU
* 2 RGB LED, 2 red LED
* ADXL345 acceleration sensor
* 1 USART port
* 1 SWD

Current progress
---------------
20140217: Finished first PCB.                
20140220: Working on the code, STM32's USB Driver is horrible.            
20140319: Abandon USB port, too complicated.         
20140329: Finished second PCB, abandon USB port, add ADXL234 sensor, and boost circuit.           
20140410：Still working on software, just break light.            
20140506：Barely work, working on reduce power consumption.
20140515：Standby power consumption down to 66 mA, Adjusting processing logic about acceleration value.
20140601: Finish adjusting logic. After several weeks trail, it's basically good.

Development blog
---------------
<http://lsgeek.blogspot.com>

