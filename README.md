# ti-mspm0-car

English | [简体中文](README_CN.md)

Based on the Texas Instruments MSPM0L1306, here is a simple car code project.

[2024 Electrical Competition Problem H Project](https://github.com/sfxfs/ti-cup-2024-h)

*Note: This project was written during the summer of 2023. Please be aware of any updates to the M0 SDK related APIs when using.*

The source code is located in the `src` folder and includes the following functionalities:

- Motor driver (motor)
- Encoder driver (encoder)
- I2C application layer functions (i2c_app)
- Reading data from JY901 attitude sensor (jy901)
- SSD1306 screen driver (OLED)
- Simple PID speed control (pid)
- Serial printf function (uart_printf)
- Delay functions and functions to get runtime (time)

The project includes a test main program, and the Ti Syscfg configuration file needs to be imported along with it.