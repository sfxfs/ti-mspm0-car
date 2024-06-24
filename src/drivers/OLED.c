#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "i2c_app.h"
#include "OLED.h"
#include "OLED_Font.h"

static const uint8_t cmd = 0x00;
static const uint8_t data = 0x40;
static uint8_t OLED_GRAM[128][8]; //128列(x)8行(y)   单位是字节
static char PRINTF_BUF[128] = { 0 };

// 发送一个字节
// mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t mode)
{
    if (mode)
    {
        i2c_app_write(OLED_INST, 0x3C, data, &dat, 1);
    }
    else
    {
        i2c_app_write(OLED_INST, 0x3C, cmd, &dat, 1);
    }
}

//清屏函数
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0;  //清除所有数据
        }
    }
    OLED_Refresh_Gram();  //更新显示
}

//更新显存到oled
//从左到右，一列一列的写。写完第一页 写第二页
void OLED_Refresh_Gram(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);      //设置显示位置—列低地址高四位固定，起作用的只有后四位
        OLED_WR_Byte(0x10, OLED_CMD);      //设置显示位置—列高地址   0000 0000也就是从0列开始
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}
void OLED_Init(void)
{
    OLED_WR_Byte(0xAE, OLED_CMD); /*display off*/
    OLED_WR_Byte(0x00, OLED_CMD); /*set lower column address*/
    OLED_WR_Byte(0x10, OLED_CMD); /*set higher column address*/
    OLED_WR_Byte(0x00, OLED_CMD); /*set display start line*/
    OLED_WR_Byte(0xB0, OLED_CMD); /*set page address*/
    OLED_WR_Byte(0x81, OLED_CMD); /*contract control*/
    OLED_WR_Byte(0xff, OLED_CMD); /*128*/
    OLED_WR_Byte(0xA1, OLED_CMD); /*set segment remap*/
    OLED_WR_Byte(0xA6, OLED_CMD); /*normal / reverse*/
    OLED_WR_Byte(0xA8, OLED_CMD); /*multiplex ratio*/
    OLED_WR_Byte(0x1F, OLED_CMD); /*duty = 1/32*/
    OLED_WR_Byte(0xC8, OLED_CMD); /*Com scan direction*/
    OLED_WR_Byte(0xD3, OLED_CMD); /*set display offset*/
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xD5, OLED_CMD); /*set osc division*/
    OLED_WR_Byte(0x80, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD); /*set pre-charge period*/
    OLED_WR_Byte(0x1f, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD); /*set COM pins*/
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xdb, OLED_CMD); /*set vcomh*/
    OLED_WR_Byte(0x40, OLED_CMD);
    OLED_WR_Byte(0x8d, OLED_CMD); /*set charge pump enable*/
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_Clear();
    OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
}

void OLED_Printf(uint8_t x, uint8_t y, uint8_t size1, uint8_t mode, char *fmt,
                 ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(PRINTF_BUF, fmt, ap);
    va_end(ap);
    OLED_ShowString(x, y, PRINTF_BUF, size1, mode);
}

//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
//mode:0,反色显示;1,正常显示
void OLED_ShowString(uint8_t x, uint8_t y, const char *chr, uint8_t size1,
                     uint8_t mode)
{
    while ((*chr >= ' ') && (*chr <= '~')) //判断是不是非法字符! 也就是是否ASCII码中 这两个字符是ASCII的最大和最小两个
    {
        OLED_ShowChar(x, y, *chr, size1, mode);
        //因为8是6*8 其他都是一半
        if (size1 == 8)
            x += 6;
        else
            x += size1 / 2;
        chr++;
    }
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24（前面的是x后面的是y）
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size1, uint8_t mode)
{
    uint8_t i, m, temp, size2, chr1;
    uint8_t x0 = x, y0 = y;
    if (size1 == 8)
        size2 = 6;
    //得到字体一个字符对应点阵集x方向所占的字节数
    //比如16号字体，那么x方向就需要1个字节，24号则需要2个字节了
    else
        size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
    chr1 = chr - ' ';  //计算偏移后的值
    for (i = 0; i < size2; i++)
    {
        if (size1 == 8)
        {
            temp = asc2_0806[chr1][i];
        } //调用0806字体
        else if (size1 == 12)
        {
            temp = asc2_1206[chr1][i];
        } //调用1206字体
        else if (size1 == 16)
        {
            temp = asc2_1608[chr1][i];
        } //调用1608字体
        else if (size1 == 24)
        {
            temp = asc2_2412[chr1][i];
        } //调用2412字体
        else
            return;
        //写一个字节 也就是一列
        for (m = 0; m < 8; m++)
        {
            if (temp & 0x01)
                OLED_DrawPoint(x, y, mode);
            else
                OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        //重新开始一页
        if ((size1 != 8) && ((x - x0) == size1 / 2))
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

//画点
//x:0~127
//y:0~63
//t:1 填充（正常显示） 0,清空
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if (t)
    {
        OLED_GRAM[x][i] |= n;
    }
    else
    {
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
        OLED_GRAM[x][i] |= n;
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    }
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1,
                      uint8_t mode)
{
    uint8_t m, temp;
    uint8_t x0 = x, y0 = y;
    uint16_t i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1; //得到字体一个字符对应点阵集所占的字节数
    for (i = 0; i < size3; i++)
    {
        if (size1 == 16)
        {
            temp = Hzk1[num][i];
        }  //调用16*16字体
        else if (size1 == 24)
        {
            temp = Hzk2[num][i];
        }  //调用24*24字体
        else if (size1 == 32)
        {
            temp = Hzk3[num][i];
        }  //调用32*32字体
        else if (size1 == 64)
        {
            temp = Hzk4[num][i];
        }  //调用64*64字体
        else
            return;
        for (m = 0; m < 8; m++)
        {
            if (temp & 0x01)
                OLED_DrawPoint(x, y, mode);
            else
                OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if ((x - x0) == size1)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

//num 显示汉字的个数
//space 每一遍显示的间隔
//mode:0,反色显示;1,正常显示
void OLED_ScrollDisplay(uint8_t num, uint8_t space, uint8_t mode)
{
    uint8_t i, n, t = 0, m = 0, r;
    while (1)
    {
        if (m == 0)
        {
            OLED_ShowChinese(128, 8, t, 16, mode); //写入一个汉字保存在OLED_GRAM[][]数组中
            t++;
        }
        if (t == num)
        {
            for (r = 0; r < 16 * space; r++)      //显示间隔
            {
                for (i = 1; i < 144; i++)
                {
                    for (n = 0; n < 4; n++)
                    {
                        OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
                    }
                }
                OLED_Refresh_Gram();
            }
            t = 0;
        }
        m++;
        if (m == 16)
        {
            m = 0;
        }
        for (i = 1; i < 144; i++)   //实现左移
        {
            for (n = 0; n < 4; n++)
            {
                OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
            }
        }
        OLED_Refresh_Gram();
    }
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey,
                      uint8_t BMP[], uint8_t mode)
{
    uint16_t j = 0;
    uint8_t i, n, temp, m;
    uint8_t x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++)
    {
        for (i = 0; i < sizex; i++)
        {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++)
            {
                if (temp & 0x01)
                    OLED_DrawPoint(x, y, mode);
                else
                    OLED_DrawPoint(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == sizex)
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}
uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}

//显示数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                  uint8_t size1, uint8_t mode)
{
    uint8_t t, temp, m = 0;
    if (size1 == 8)
        m = 2;
    for (t = 0; t < len; t++)
    {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if (temp == 0)
        {
            OLED_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
        }
        else
        {
            OLED_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
        }
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1; //画线起点坐标
    uCol = y1;
    if (delta_x > 0)
        incx = 1; //设置单步方向
    else if (delta_x == 0)
        incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_x;
    }
    if (delta_x > delta_y)
        distance = delta_x; //选取基本增量坐标轴
    else
        distance = delta_y;
    for (t = 0; t < distance + 1; t++)
    {
        OLED_DrawPoint(uRow, uCol, mode); //画点
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }

}

//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r)
{
    int a, b, num;
    a = 0;
    b = r;
    while (2 * b * b >= r * r)
    {
        OLED_DrawPoint(x + a, y - b, 1);
        OLED_DrawPoint(x - a, y - b, 1);
        OLED_DrawPoint(x - a, y + b, 1);
        OLED_DrawPoint(x + a, y + b, 1);

        OLED_DrawPoint(x + b, y + a, 1);
        OLED_DrawPoint(x + b, y - a, 1);
        OLED_DrawPoint(x - b, y - a, 1);
        OLED_DrawPoint(x - b, y + a, 1);

        a++;
        num = (a * a + b * b) - r * r; //计算画的点离圆心的距离
        if (num > 0)
        {
            b--;
            a--;
        }
    }
}

//开启OLED显示
void OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD); //电荷泵使能
    OLED_WR_Byte(0x14, OLED_CMD); //开启电荷泵
    OLED_WR_Byte(0xAF, OLED_CMD); //点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD); //电荷泵使能
    OLED_WR_Byte(0x10, OLED_CMD); //关闭电荷泵
    OLED_WR_Byte(0xAE, OLED_CMD); //关闭屏幕
}

//反显函数
void OLED_ColorTurn(uint8_t i)
{
    if (i == 0)
    {
        OLED_WR_Byte(0xA6, OLED_CMD); //正常显示
    }
    if (i == 1)
    {
        OLED_WR_Byte(0xA7, OLED_CMD); //反色显示
    }
}

//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
    if (i == 0)
    {
        OLED_WR_Byte(0xC8, OLED_CMD); //正常显示
        OLED_WR_Byte(0xA1, OLED_CMD);
    }
    if (i == 1)
    {
        OLED_WR_Byte(0xC0, OLED_CMD); //反转显示
        OLED_WR_Byte(0xA0, OLED_CMD);
    }
}
