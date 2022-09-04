/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-03     SkyLin       the first version
 */
#include "include/lcd.h"
#include "include/font.h"

static int lcd_spi_hw_init(void)
{
    rt_err_t ret;
    /**********************将LCD挂载到SPI1上*********************/
    ret = rt_hw_spi_device_attach("spi1", "spi10", GPIOA, GPIO_PIN_4);
    if (ret != RT_EOK)
    {
        rt_kprintf("rt_hw_spi_device_attach %s err ...\n", "spi10");
        return -RT_EOK;
    }

    return RT_EOK;
}


/**
 * LCD构造函数
 * @param spiName
 * @param dcPin
 * @param resPin
 */
Lcd::Lcd(const char* spiName, rt_base_t dcPin, rt_base_t resPin)
{
    this->spiName = spiName;
    this->dcPin = dcPin;
    this->resPin = resPin;
}

void Lcd::WriteCmd(rt_uint8_t cmd)
{
    rt_pin_write(dcPin, PIN_LOW);
    rt_spi_send(lcd_dev, &cmd, 1);
}

void Lcd::WriteData(rt_uint8_t data)
{
    rt_pin_write(dcPin, PIN_HIGH);
    rt_spi_send(lcd_dev, &data, 1);
}

void Lcd::WriteData16(rt_uint16_t data)
{
    WriteData(data >> 8);
    WriteData(data & 0xff);
}

/**
 * 清屏函数
 * @param color
 */
void Lcd::Clear(rt_uint16_t color)
{
    rt_uint16_t i, j;
    AddrSet(0, 0, LCD_Width - 1, LCD_Height - 1);
    for (i = 0; i < LCD_Width; i++)
    {
        for (j = 0; j < LCD_Height; j++)
        {
            WriteData16(color);
        }
    }
}

/**
 * lcd设置区域
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void Lcd::AddrSet(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    if (USE_HORIZONTAL == 0)
    {
        WriteCmd(0x2a); //列地址设置
        WriteData16(x1 + 52);
        WriteData16(x2 + 52);
        WriteCmd(0x2b); //行地址设置
        WriteData16(y1 + 40);
        WriteData16(y2 + 40);
        WriteCmd(0x2c); //储存器写
    }
    else if (USE_HORIZONTAL == 1)
    {
        WriteCmd(0x2a); //列地址设置
        WriteData16(x1 + 53);
        WriteData16(x2 + 53);
        WriteCmd(0x2b); //行地址设置
        WriteData16(y1 + 40);
        WriteData16(y2 + 40);
        WriteCmd(0x2c); //储存器写
    }
    else if (USE_HORIZONTAL == 2)
    {
        WriteCmd(0x2a); //列地址设置
        WriteData16(x1 + 40);
        WriteData16(x2 + 40);
        WriteCmd(0x2b); //行地址设置
        WriteData16(y1 + 53);
        WriteData16(y2 + 53);
        WriteCmd(0x2c); //储存器写
    }
    else
    {
        WriteCmd(0x2a); //列地址设置
        WriteData16(x1 + 40);
        WriteData16(x2 + 40);
        WriteCmd(0x2b); //行地址设置
        WriteData16(y1 + 52);
        WriteData16(y2 + 52);
        WriteCmd(0x2c); //储存器写
    }
}

/**
 * 字符绘制
 * @param x
 * @param y
 * @param dat
 */
void Lcd::ShowChar(rt_uint16_t x, rt_uint16_t y, const char dat)
{
    rt_uint8_t i, j;
    rt_uint8_t temp;

    for (i = 0; i < 16; i++)
    {
        AddrSet(x, y + i, x + 7, y + i);
        temp = tft_ascii[dat - 32][i]; //减32因为是取模是从空格开始取得 空格在ascii中序号是32
        for (j = 0; j < 8; j++)
        {
            if (temp & 0x01)
            {
                WriteData16(CYAN);
            }
            else
                WriteData16(BLACK);
            temp >>= 1;
        }
    }
}

/**
 * 字符串绘制
 * @param x
 * @param y
 * @param dat
 */
void Lcd::ShowStr(rt_uint16_t x, rt_uint16_t y, const char dat[])
{
    rt_uint16_t j;

    j = 0;
    while (dat[j] != '\0')
    {
        ShowChar(x + 8 * j, y * 16, dat[j]);
        j++;
    }
}

/**
 * LCD初始化
 */
void Lcd::Init()
{
    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
    cfg.max_hz = 40 * 1000 * 1000;

    lcd_dev = (struct rt_spi_device *) rt_device_find(spiName);

    if (lcd_dev == NULL)
    {
        rt_kprintf("find spi device failed\n");
    }
    else
    {
        rt_spi_configure(lcd_dev, &cfg);

    }

    rt_pin_mode(dcPin, PIN_MODE_OUTPUT);
    rt_pin_mode(resPin, PIN_MODE_OUTPUT);

    /* 复位 */
    rt_pin_write(resPin, PIN_LOW);
    rt_thread_delay(100);
    rt_pin_write(resPin, PIN_HIGH);

    /* 关闭睡眠模式 */
    WriteCmd(0x11);
    rt_thread_delay(120);

    /* 开始设置显存扫描模式，数据格式等 */
    WriteCmd(0x36);
    if (USE_HORIZONTAL == 0)
        WriteData(0x00);
    else if (USE_HORIZONTAL == 1)
        WriteData(0xC0);
    else if (USE_HORIZONTAL == 2)
        WriteData(0x70);
    else
        WriteData(0xA0);

    /* RGB 5-6-5-bit格式  */
    WriteCmd(0x3A);
    WriteData(0x65);
    /* porch 设置 */
    WriteCmd(0xB2);
    WriteData(0x0C);
    WriteData(0x0C);
    WriteData(0x00);
    WriteData(0x33);
    WriteData(0x33);
    /* VGH设置 */
    WriteCmd(0xB7);
    WriteData(0x72);
    /* VCOM 设置 */
    WriteCmd(0xBB);
    WriteData(0x3D);
    /* LCM 设置 */
    WriteCmd(0xC0);
    WriteData(0x2C);
    /* VDV and VRH 设置 */
    WriteCmd(0xC2);
    WriteData(0x01);
    /* VRH 设置 */
    WriteCmd(0xC3);
    WriteData(0x19);
    /* VDV 设置 */
    WriteCmd(0xC4);
    WriteData(0x20);
    /* 普通模式下显存速率设置 60Mhz */
    WriteCmd(0xC6);
    WriteData(0x0F);
    /* 电源控制 */
    WriteCmd(0xD0);
    WriteData(0xA4);
    WriteData(0xA1);
    /* 电压设置 */
    WriteCmd(0xE0);
    WriteData(0xD0);
    WriteData(0x04);
    WriteData(0x0D);
    WriteData(0x11);
    WriteData(0x13);
    WriteData(0x2B);
    WriteData(0x3F);
    WriteData(0x54);
    WriteData(0x4C);
    WriteData(0x18);
    WriteData(0x0D);
    WriteData(0x0B);
    WriteData(0x1F);
    WriteData(0x23);
    /* 电压设置 */
    WriteCmd(0xE1);
    WriteData(0xD0);
    WriteData(0x04);
    WriteData(0x0C);
    WriteData(0x11);
    WriteData(0x13);
    WriteData(0x2C);
    WriteData(0x3F);
    WriteData(0x44);
    WriteData(0x51);
    WriteData(0x2F);
    WriteData(0x1F);
    WriteData(0x1F);
    WriteData(0x20);
    WriteData(0x23);
    /* 显示开 */
    WriteCmd(0x21);
    WriteCmd(0x29);

    AddrSet(0, 0, LCD_Width - 1, LCD_Height - 1);

    //清屏
    Clear(BLACK);
}

Lcd::~Lcd()
{
    // TODO 自动生成的析构函数存根
}

INIT_DEVICE_EXPORT(lcd_spi_hw_init);
