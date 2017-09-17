#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

#include "SC1035_sensor_ctl.h"

const unsigned char sensor_i2c_addr	=	0x60;		/* I2C Address of AR0130 */
const unsigned int  sensor_addr_byte	=	2;
const unsigned int  sensor_data_byte	=	1;

int sensor_read_register(int addr)
{
	// TODO: 
	
	return 0;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;
    
    fd = open("/dev/gpioi2c_ex", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ex error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#else
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;
	
    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }
    
    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("hi_i2c write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#endif
	return 0;
}

static void delay_ms(int ms) { 
    usleep(ms*1000);
}

// void sensor_prog(int* rom) 
// {
    // int i = 0;
    // while (1) {
        // int lookup = rom[i++];
        // int addr = (lookup >> 16) & 0xFFFF;
        // int data = lookup & 0xFFFF;
        // if (addr == 0xFFFE) {
            // delay_ms(data);
        // } else if (addr == 0xFFFF) {
            // return;
        // } else {
            // sensor_write_register(addr, data);
        // }
    // }
// }

void sensor_init()
{

    sensor_write_register(0x3208, 0x5); // TIMING_X_OUTPUT_SIZE (pg 53)
	sensor_write_register(0x3000, 0x1); // SC_REG00
	sensor_write_register(0x3003, 0x1);
	sensor_write_register(0x3400, 0x53); 
	sensor_write_register(0x3416, 0xc0); // RNC_CTRL16 (pg 63)
	sensor_write_register(0x3d08, 0x0); // DVP_POL_CTRL (pg 73)
	sensor_write_register(0x5000, 0x9); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3e03, 0x3);
	sensor_write_register(0x3928, 0x0); // BLC_CTRL28 (pg 61)
	sensor_write_register(0x3630, 0x58); // PWC[7:0] (pg 46)
	sensor_write_register(0x3612, 0x0); // ANALOG[23:16] (pg 45)
	sensor_write_register(0x3632, 0x41); // PWC[23:16] (pg 46)
	sensor_write_register(0x3635, 0x4); // PWC[47:40] (pg 46)
	sensor_write_register(0x3500, 0x10);
	sensor_write_register(0x3631, 0x80); // PWC[15:8] (pg 46)
	sensor_write_register(0x3620, 0x44);
	sensor_write_register(0x3633, 0x7c); // PWC[31:24] (pg 46)
	sensor_write_register(0x3780, 0xb);
	sensor_write_register(0x3300, 0x33); // SENSOR_REG00 (pg 46) = 0b00110011
	sensor_write_register(0x3301, 0x38); // SENSOR_REG00 (pg 46) = 0b00111000
	sensor_write_register(0x3302, 0x30);
	sensor_write_register(0x3303, 0x66); //SENSOR_HLDWIDTH (pg 46) [hblk/hsig width] 0x66 = 102
	sensor_write_register(0x3304, 0xa0); // SENSOR_TXWIDTH (pg 46) [tx width_sa] 0xa0 = 160
	sensor_write_register(0x3305, 0x72); // SENSOR_REG05 (pg 46)
	sensor_write_register(0x331e, 0x56);
	sensor_write_register(0x321e, 0x0);
	sensor_write_register(0x321f, 0xa);
	sensor_write_register(0x3216, 0xa);
	sensor_write_register(0x3115, 0xa);
	sensor_write_register(0x3332, 0x38);
	sensor_write_register(0x5054, 0x82);
	sensor_write_register(0x3622, 0x26);
	sensor_write_register(0x3907, 0x2);
	sensor_write_register(0x3908, 0x0);
	sensor_write_register(0x3601, 0x18);
	sensor_write_register(0x3315, 0x44);
	sensor_write_register(0x3308, 0x40);
	sensor_write_register(0x3223, 0x22);
	sensor_write_register(0x3e0e, 0x50); // AEC_PK_CTRL0E (pg 44)
	sensor_write_register(0x3101, 0x9b);
	sensor_write_register(0x3114, 0x3); // R_FPN_ROW_END_H (pg 37) 0x3d1 = 977
	sensor_write_register(0x3115, 0xd1); // R_FPN_ROW_END_L (pg 37)
	sensor_write_register(0x3211, 0x60); // TIMING_HOFFS
	sensor_write_register(0x5780, 0xff);
	sensor_write_register(0x5781, 0x7f);
	sensor_write_register(0x5785, 0xff);
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3631, 0x82);  // PWC[15:8] (pg 46)
	
	// Page 14
	
	sensor_write_register(0x3010, 0x7); // PLL_CTRL0
	                                   // Bit[7] pll_bypass
									   // Bit[6:4] RESERVED
									   // Bit[3:1] PREDIV[2:0]
									   // Bit[0] PLLDIV[5:4]
									   // 0x7 = 00000111
	sensor_write_register(0x3011, 0x46); // PLL_CTRL1
									    //Bit[7:3] PLLDIV[4:0] Multiplier = 64 - PLLDIV[5:0]
									    //Bit[2:0] SYSEL[1:0]
									    // 0x46 = 01000110
	// pll_bypass = 0b0 = 0
	// PREDIV = 0b011 = ~3x
	// PLLDIV = 101000 = 40
	// SYSEL = 0b01 = ~4x
	
	// Multiplier = 64 - 40 = 24

	// ((27/3) * 24) / 4 = 54MHz?
	
	// PLL control SC1035 PLL allows input clock frequency range of 6 ~ 27MHz,
	// VCO frequency which supports up to 500MHz.
	
	sensor_write_register(0x3004, 0x4); // SC_REG04
	sensor_write_register(0x3610, 0x2b); // ANALOG[7:0] (pg 45)
	
	// https://design.ece.msstate.edu/2015/team_rogers/datasheets/RaspberryPiCameraModule.pdf
	
	// VTS info
	// http://www.ghgtechn.com/admin/upload/634912807181563750OV5656_.1_KING%20HORN%20ENTERPRISES%20Ltd..pdf
	// http://www.eevblog.com/forum/microcontrollers/ov5640-help-needed!-no-pclk-signal/
	
	sensor_write_register(0x320c, 0x7); // TIMING_HTS (pg 53) [horizontal total size] 0x708 = 1800
	sensor_write_register(0x320d, 0x8); // TIMING_HTS (pg 53)
	sensor_write_register(0x320e, 0x3); // TIMING_VTS (pg 53) 0x3e8 = 1000
	sensor_write_register(0x320f, 0xe8); // TIMING_VTS (pg 53) [vertical total size]
	sensor_write_register(0x3210, 0x0); // TIMING_HOFFS - high (pg 53) 0x60 = 96 [win_hoffs]
	sensor_write_register(0x3211, 0x60); // TIMING_HOFFS - high (pg 53)
	sensor_write_register(0x3212, 0x0); // TIMING_VOFFS - high (pg 53) 0x81 = 129
	sensor_write_register(0x3213, 0x81); // TIMING_VOFFS - low (pg 53)
	sensor_write_register(0x3208, 0x5); // TIMING_X_OUTPUT_SIZE (pg 53) 0x500 = 1280
	sensor_write_register(0x3209, 0x0); // TIMING_X_OUTPUT_SIZE (pg 53)
	sensor_write_register(0x320a, 0x2); // TIMING_Y_OUTPUT_SIZE (pg 53) 0x2d0 = 720
	sensor_write_register(0x320b, 0xd0); // TIMING_Y_OUTPUT_SIZE (pg 53)
	sensor_write_register(0x3202, 0x0); // TIMING_Y_START_ADDR (pg 52) 0x8 = 8
	sensor_write_register(0x3203, 0x8); // TIMING_Y_START_ADDR (pg 52)
	sensor_write_register(0x3206, 0x3); // TIMING_Y_END_ADDR - high (pg 53) 0x3cf = 975
	sensor_write_register(0x3207, 0xcf); // TIMING_Y_END_ADDR - low (pg 53)
	
	sensor_write_register(0x3330, 0xd);
	sensor_write_register(0x3320, 0x6);
	sensor_write_register(0x3321, 0xe8);
	sensor_write_register(0x3322, 0x1);
	sensor_write_register(0x3323, 0xc0);
	sensor_write_register(0x3600, 0x54); // ADC[7:0] (pg 45)
	sensor_write_register(0x320c, 0x8);
	sensor_write_register(0x320d, 0x70);
	sensor_write_register(0x3320, 0x8);
	sensor_write_register(0x3321, 0x50);
	sensor_write_register(0x320e, 0x3);
	sensor_write_register(0x320f, 0xe8);
	sensor_write_register(0x331e, 0xe0); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xe0);
	sensor_write_register(0x331e, 0xbe);
	sensor_write_register(0x331e, 0x8f);
	sensor_write_register(0x331e, 0x5c);
	sensor_write_register(0x331e, 0x24);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51) [width_pchg]
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x320e, 0x7);
	sensor_write_register(0x320f, 0xd0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x320e, 0x3);
	sensor_write_register(0x320f, 0xe8);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x320e, 0x7);
	sensor_write_register(0x320f, 0xd0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x320e, 0x3);
	sensor_write_register(0x320f, 0xe8);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1c); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x320e, 0x7);
	sensor_write_register(0x320f, 0xd0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x5000, 0x0); // ISP_CTRL00 (pg 28 or pg 68) [LENC correction enable]
	sensor_write_register(0x3416, 0xc0);
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)
	sensor_write_register(0x331e, 0x1b); // SENSOR_TXWIDTH (pg 51)

    printf("Aptina AR0130 sensor 720P30fps init success!\n");
    
}


