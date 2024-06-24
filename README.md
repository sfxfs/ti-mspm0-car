# ti-mspm0-car
基于德州仪器的 MSPM0L1306 的简易小车代码

*注：本项目代码写于 2023 年暑假，使用请注意 M0 SDK 相关 API 的更新*

源代码位于 `src` 文件夹，实现了以下功能：

- 电机驱动（motor）
- 编码器驱动（encoder）
- I2C 应用层函数（i2c_app）
- JY901 姿态传感器读取（jy901）
- SSD1306 屏幕驱动（OLED）
- 简易 PID 速度控制（pid）
- 串口 printf 打印（uart_printf）
- 延时函数以及获取运行时间的函数（time）

附带测试的 main 程序，以及 Ti Syscfg 配置文件需要一并导入。