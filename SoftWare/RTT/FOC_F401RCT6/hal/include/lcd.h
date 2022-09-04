/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-03     SkyLin       the first version
 */
#ifndef HAL_LCD_H_
#define HAL_LCD_H_

extern "C"
{
#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
#include "drv_spi.h"
}

#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色
#define GRAYBLUE         0X5458 //灰蓝色
#define LIGHTGREEN       0X841F //浅绿色
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


//LCD的宽和高定义
#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_Width 135
#define LCD_Height 240

#else
#define LCD_Width 240
#define LCD_Height 135
#endif

#define LCD_TOTAL_BUF_SIZE  (240*135*2)
#define LCD_Buf_Size 1152

class Lcd
{
public:
    Lcd() = default;
    Lcd(const char* spiName, rt_base_t dcPin, rt_base_t resPin);
    void Init();
    void WriteCmd(rt_uint8_t cmd);
    void WriteData(rt_uint8_t data);
    void WriteData16(rt_uint16_t data);
    void Clear(rt_uint16_t color);
    void AddrSet(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2);
    void ShowChar(rt_uint16_t x,rt_uint16_t y,const char dat);
    void ShowStr(rt_uint16_t x,rt_uint16_t y,const char dat[]);
    ~Lcd();
private:
    struct rt_spi_device *lcd_dev;
    const char* spiName;
    rt_base_t dcPin;
    rt_base_t resPin;
};

#endif /* HAL_LCD_H_ */
