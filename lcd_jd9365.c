/* drivers/video/sc8825/lcd_jd9365_mipi.c
 *
 * Support for nt35516 mipi LCD device
 *
 * Copyright (C) 2010 Spreadtrum
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include "../sprdfb_panel.h"

//#define LCD_Delay(ms)  uDelay(ms*1000)
//#define  LCD_DEBUG
#ifdef LCD_DEBUG
#define LCD_PRINT printk
#else
#define LCD_PRINT(...)
#endif

#define MAX_DATA   48

typedef struct LCM_Init_Code_tag {
	unsigned int tag;
	unsigned char data[MAX_DATA];
}LCM_Init_Code;

typedef struct LCM_force_cmd_code_tag{
	unsigned int datatype;
	LCM_Init_Code real_cmd_code;
}LCM_Force_Cmd_Code;

#define LCM_TAG_SHIFT 24
#define LCM_TAG_MASK  ((1 << 24) -1)
#define LCM_SEND(len) ((1 << LCM_TAG_SHIFT)| len)
#define LCM_SLEEP(ms) ((2 << LCM_TAG_SHIFT)| ms)
//#define ARRAY_SIZE(array) ( sizeof(array) / sizeof(array[0]))

#define LCM_TAG_SEND  (1<< 0)
#define LCM_TAG_SLEEP (1 << 1)

static LCM_Init_Code init_data[] = {
//Page0
{LCM_SEND(4), {2, 0,0xE0,0x00}},
{LCM_SEND(4), {2, 0,0xE1,0x93}},//--- PASSWORD  ----//
{LCM_SEND(4), {2, 0,0xE2,0x65}},
{LCM_SEND(4), {2, 0,0xE3,0xF8}},

{LCM_SEND(4), {2, 0,0x70,0x20}},//--- Sequence Ctrl ----//
{LCM_SEND(4), {2, 0,0x71,0x13}},
{LCM_SEND(4), {2, 0,0x72,0x06}},
{LCM_SEND(4), {2, 0,0x75,0x03}},

{LCM_SEND(4), {2, 0,0xE0,0x01}},//Page1  

{LCM_SEND(4), {2, 0,0x00,0x00}},//Set VCOM
{LCM_SEND(4), {2, 0,0x01,0xA9}},//zyp change a8 -> a9 for flick
{LCM_SEND(4), {2, 0,0x03,0x00}},
{LCM_SEND(4), {2, 0,0x04,0xA8}},
{LCM_SEND(4), {2, 0,0x0C,0x74}},

{LCM_SEND(4), {2, 0,0x17,0x00}},//Set Gamma Power,1,{ VGMP,1,{VGMN,1,{VGSP,1,{VGSN
{LCM_SEND(4), {2, 0,0x18,0xB7}},
{LCM_SEND(4), {2, 0,0x19,0x00}},
{LCM_SEND(4), {2, 0,0x1A,0x00}},
{LCM_SEND(4), {2, 0,0x1B,0xB7}},
{LCM_SEND(4), {2, 0,0x1C,0x00}},

{LCM_SEND(4), {2, 0,0x1F,0x7E}},//Set Gate Power
{LCM_SEND(4), {2, 0,0x20,0x24}},
{LCM_SEND(4), {2, 0,0x21,0x24}},
{LCM_SEND(4), {2, 0,0x22,0x4E}},
{LCM_SEND(4), {2, 0,0x24,0xF8}},

{LCM_SEND(4), {2, 0,0x37,0x09}},//SET RGBCYC
{LCM_SEND(4), {2, 0,0x38,0x04}},
{LCM_SEND(4), {2, 0,0x3D,0xFF}},
{LCM_SEND(4), {2, 0,0x3E,0xFF}},
{LCM_SEND(4), {2, 0,0x3F,0x7F}},

{LCM_SEND(4), {2, 0,0x40,0x04}},//Set TCON
{LCM_SEND(4), {2, 0,0x41,0xA0}},

{LCM_SEND(4), {2, 0,0x55,0x01}},//--- power voltage  ----//
{LCM_SEND(4), {2, 0,0x56,0x01}},
{LCM_SEND(4), {2, 0,0x57,0x69}},
{LCM_SEND(4), {2, 0,0x58,0x0A}},
{LCM_SEND(4), {2, 0,0x59,0x2A}},
{LCM_SEND(4), {2, 0,0x5A,0x1B}},
{LCM_SEND(4), {2, 0,0x5B,0x18}},

{LCM_SEND(4), {2, 0,0x5D,0x68}},//--- Gamma  ----//
{LCM_SEND(4), {2, 0,0x5E,0x44}},
{LCM_SEND(4), {2, 0,0x5F,0x35}},
{LCM_SEND(4), {2, 0,0x60,0x2A}},
{LCM_SEND(4), {2, 0,0x61,0x27}},
{LCM_SEND(4), {2, 0,0x62,0x19}},
{LCM_SEND(4), {2, 0,0x63,0x20}},
{LCM_SEND(4), {2, 0,0x64,0x0D}},
{LCM_SEND(4), {2, 0,0x65,0x27}},
{LCM_SEND(4), {2, 0,0x66,0x27}},
{LCM_SEND(4), {2, 0,0x67,0x28}},
{LCM_SEND(4), {2, 0,0x68,0x48}},
{LCM_SEND(4), {2, 0,0x69,0x37}},
{LCM_SEND(4), {2, 0,0x6A,0x45}},
{LCM_SEND(4), {2, 0,0x6B,0x3B}},
{LCM_SEND(4), {2, 0,0x6C,0x3C}},
{LCM_SEND(4), {2, 0,0x6D,0x2A}},
{LCM_SEND(4), {2, 0,0x6E,0x25}},
{LCM_SEND(4), {2, 0,0x6F,0x0F}},

{LCM_SEND(4), {2, 0,0x70,0x78}},//--- Gamma  ----//
{LCM_SEND(4), {2, 0,0x71,0x44}},
{LCM_SEND(4), {2, 0,0x72,0x35}},
{LCM_SEND(4), {2, 0,0x73,0x2A}},
{LCM_SEND(4), {2, 0,0x74,0x27}},
{LCM_SEND(4), {2, 0,0x75,0x19}},
{LCM_SEND(4), {2, 0,0x76,0x20}},
{LCM_SEND(4), {2, 0,0x77,0x0D}},
{LCM_SEND(4), {2, 0,0x78,0x27}},
{LCM_SEND(4), {2, 0,0x79,0x27}},
{LCM_SEND(4), {2, 0,0x7A,0x28}},
{LCM_SEND(4), {2, 0,0x7B,0x48}},
{LCM_SEND(4), {2, 0,0x7C,0x37}},
{LCM_SEND(4), {2, 0,0x7D,0x45}},
{LCM_SEND(4), {2, 0,0x7E,0x3B}},
{LCM_SEND(4), {2, 0,0x7F,0x3C}},
{LCM_SEND(4), {2, 0,0x80,0x2A}},
{LCM_SEND(4), {2, 0,0x81,0x25}},
{LCM_SEND(4), {2, 0,0x82,0x0F}},

{LCM_SEND(4), {2, 0,0xE0,0x02}},//Page2,1,{ for GIP

{LCM_SEND(4), {2, 0,0x00,0x45}},//GIP_L Pin mapping
{LCM_SEND(4), {2, 0,0x01,0x47}},
{LCM_SEND(4), {2, 0,0x02,0x49}},
{LCM_SEND(4), {2, 0,0x03,0x4B}},
{LCM_SEND(4), {2, 0,0x04,0x41}},
{LCM_SEND(4), {2, 0,0x05,0x43}},
{LCM_SEND(4), {2, 0,0x06,0x1F}},
{LCM_SEND(4), {2, 0,0x07,0x1F}},
{LCM_SEND(4), {2, 0,0x08,0x1F}},
{LCM_SEND(4), {2, 0,0x09,0x1F}},
{LCM_SEND(4), {2, 0,0x0A,0x1F}},
{LCM_SEND(4), {2, 0,0x0B,0x1F}},
{LCM_SEND(4), {2, 0,0x0C,0x1F}},
{LCM_SEND(4), {2, 0,0x0D,0x1F}},
{LCM_SEND(4), {2, 0,0x0E,0x1F}},
{LCM_SEND(4), {2, 0,0x0F,0x1E}},
{LCM_SEND(4), {2, 0,0x10,0x1E}},
{LCM_SEND(4), {2, 0,0x11,0x1E}},
{LCM_SEND(4), {2, 0,0x12,0x1F}},
{LCM_SEND(4), {2, 0,0x13,0x1F}},
{LCM_SEND(4), {2, 0,0x14,0x1E}},
{LCM_SEND(4), {2, 0,0x15,0x1F}},

{LCM_SEND(4), {2, 0,0x16,0x44}},//GIP_R Pin mapping
{LCM_SEND(4), {2, 0,0x17,0x46}},
{LCM_SEND(4), {2, 0,0x18,0x48}},
{LCM_SEND(4), {2, 0,0x19,0x4A}},
{LCM_SEND(4), {2, 0,0x1A,0x40}},
{LCM_SEND(4), {2, 0,0x1B,0x42}},
{LCM_SEND(4), {2, 0,0x1C,0x1F}},
{LCM_SEND(4), {2, 0,0x1D,0x1F}},
{LCM_SEND(4), {2, 0,0x1E,0x1F}},
{LCM_SEND(4), {2, 0,0x1F,0x1F}},
{LCM_SEND(4), {2, 0,0x20,0x1F}},
{LCM_SEND(4), {2, 0,0x21,0x1F}},
{LCM_SEND(4), {2, 0,0x22,0x1F}},
{LCM_SEND(4), {2, 0,0x23,0x1F}},
{LCM_SEND(4), {2, 0,0x24,0x1F}},
{LCM_SEND(4), {2, 0,0x25,0x1E}},
{LCM_SEND(4), {2, 0,0x26,0x1E}},
{LCM_SEND(4), {2, 0,0x27,0x1E}},
{LCM_SEND(4), {2, 0,0x28,0x1F}},
{LCM_SEND(4), {2, 0,0x29,0x1F}},
{LCM_SEND(4), {2, 0,0x2A,0x1E}},
{LCM_SEND(4), {2, 0,0x2B,0x1F}},

{LCM_SEND(4), {2, 0,0x2C,0x0A}},//GIP_L_GS Pin mapping
{LCM_SEND(4), {2, 0,0x2D,0x08}},
{LCM_SEND(4), {2, 0,0x2E,0x06}},
{LCM_SEND(4), {2, 0,0x2F,0x04}},
{LCM_SEND(4), {2, 0,0x30,0x02}},
{LCM_SEND(4), {2, 0,0x31,0x00}},
{LCM_SEND(4), {2, 0,0x32,0x1F}},
{LCM_SEND(4), {2, 0,0x33,0x1F}},
{LCM_SEND(4), {2, 0,0x34,0x1F}},
{LCM_SEND(4), {2, 0,0x35,0x1F}},
{LCM_SEND(4), {2, 0,0x36,0x1F}},
{LCM_SEND(4), {2, 0,0x37,0x1F}},
{LCM_SEND(4), {2, 0,0x38,0x1F}},
{LCM_SEND(4), {2, 0,0x39,0x1F}},
{LCM_SEND(4), {2, 0,0x3A,0x1F}},
{LCM_SEND(4), {2, 0,0x3B,0x1E}},
{LCM_SEND(4), {2, 0,0x3C,0x1E}},
{LCM_SEND(4), {2, 0,0x3D,0x1E}},
{LCM_SEND(4), {2, 0,0x3E,0x1F}},
{LCM_SEND(4), {2, 0,0x3F,0x1F}},
{LCM_SEND(4), {2, 0,0x40,0x1F}},
{LCM_SEND(4), {2, 0,0x41,0x1E}},

{LCM_SEND(4), {2, 0,0x42,0x0B}},//GIP_R_GS Pin mapping
{LCM_SEND(4), {2, 0,0x43,0x09}},
{LCM_SEND(4), {2, 0,0x44,0x07}},
{LCM_SEND(4), {2, 0,0x45,0x05}},
{LCM_SEND(4), {2, 0,0x46,0x03}},
{LCM_SEND(4), {2, 0,0x47,0x01}},
{LCM_SEND(4), {2, 0,0x48,0x1F}},
{LCM_SEND(4), {2, 0,0x49,0x1F}},
{LCM_SEND(4), {2, 0,0x4A,0x1F}},
{LCM_SEND(4), {2, 0,0x4B,0x1F}},
{LCM_SEND(4), {2, 0,0x4C,0x1F}},
{LCM_SEND(4), {2, 0,0x4D,0x1F}},
{LCM_SEND(4), {2, 0,0x4E,0x1F}},
{LCM_SEND(4), {2, 0,0x4F,0x1F}},
{LCM_SEND(4), {2, 0,0x50,0x1F}},
{LCM_SEND(4), {2, 0,0x51,0x1E}},
{LCM_SEND(4), {2, 0,0x52,0x1E}},
{LCM_SEND(4), {2, 0,0x53,0x1E}},
{LCM_SEND(4), {2, 0,0x54,0x1F}},
{LCM_SEND(4), {2, 0,0x55,0x1F}},
{LCM_SEND(4), {2, 0,0x56,0x1F}},
{LCM_SEND(4), {2, 0,0x57,0x1E}},

{LCM_SEND(4), {2, 0,0x58,0x40}},//GIP Timing  
{LCM_SEND(4), {2, 0,0x59,0x00}},
{LCM_SEND(4), {2, 0,0x5A,0x00}},
{LCM_SEND(4), {2, 0,0x5B,0x30}},
{LCM_SEND(4), {2, 0,0x5C,0x08}},
{LCM_SEND(4), {2, 0,0x5D,0x40}},
{LCM_SEND(4), {2, 0,0x5E,0x01}},
{LCM_SEND(4), {2, 0,0x5F,0x02}},
{LCM_SEND(4), {2, 0,0x60,0x00}},
{LCM_SEND(4), {2, 0,0x61,0x01}},
{LCM_SEND(4), {2, 0,0x62,0x02}},
{LCM_SEND(4), {2, 0,0x63,0x69}},
{LCM_SEND(4), {2, 0,0x64,0x6A}},
{LCM_SEND(4), {2, 0,0x65,0x00}},
{LCM_SEND(4), {2, 0,0x66,0x00}},
{LCM_SEND(4), {2, 0,0x67,0x74}},
{LCM_SEND(4), {2, 0,0x68,0x0A}},
{LCM_SEND(4), {2, 0,0x69,0x69}},
{LCM_SEND(4), {2, 0,0x6A,0x6A}},
{LCM_SEND(4), {2, 0,0x6B,0x10}},
{LCM_SEND(4), {2, 0,0x6C,0x00}},//GIP Timing  
{LCM_SEND(4), {2, 0,0x6D,0x04}},
{LCM_SEND(4), {2, 0,0x6E,0x04}},
{LCM_SEND(4), {2, 0,0x6F,0x88}},
{LCM_SEND(4), {2, 0,0x70,0x00}},
{LCM_SEND(4), {2, 0,0x71,0x00}},
{LCM_SEND(4), {2, 0,0x72,0x06}},
{LCM_SEND(4), {2, 0,0x73,0x7B}},
{LCM_SEND(4), {2, 0,0x74,0x00}},
{LCM_SEND(4), {2, 0,0x75,0x87}},
{LCM_SEND(4), {2, 0,0x76,0x00}},
{LCM_SEND(4), {2, 0,0x77,0x5D}},
{LCM_SEND(4), {2, 0,0x78,0x17}},
{LCM_SEND(4), {2, 0,0x79,0x1F}},
{LCM_SEND(4), {2, 0,0x7A,0x00}},
{LCM_SEND(4), {2, 0,0x7B,0x00}},
{LCM_SEND(4), {2, 0,0x7C,0x00}},
{LCM_SEND(4), {2, 0,0x7D,0x03}},
{LCM_SEND(4), {2, 0,0x7E,0x7B}},

//Page4
{LCM_SEND(4), {2, 0,0xE0,0x04}},
{LCM_SEND(4), {2, 0,0x2B,0x2B}},
{LCM_SEND(4), {2, 0,0x2E,0x44}},

//Page0
{LCM_SEND(4), {2, 0,0xE0,0x00}},
{LCM_SEND(4), {2, 0,0x55,0xB0}},
{LCM_SEND(4), {2, 0,0xE6,0x02}},
{LCM_SEND(4), {2, 0,0xE7,0x02}},
{LCM_SEND(4), {2, 0,0x11,0x00}},
{LCM_SLEEP(120)},
{LCM_SEND(4), {2, 0,0x29,0x00}},
{LCM_SLEEP(20)},
{LCM_SEND(4), {2, 0,0xE0,0x00}},
{LCM_SEND(4), {2, 0,0xE1,0x09}},
{LCM_SEND(4), {2, 0,0xE2,0xB1}},
{LCM_SEND(4), {2, 0,0xE3,0x7F}},
                                                                               
};

//static LCM_Init_Code disp_on =  {LCM_SEND(1), {0x29}};

static LCM_Init_Code sleep_in[] =  {
    {LCM_SEND(2), {0xE0,0x00}},
    {LCM_SEND(2), {0xE1,0x93}},
    {LCM_SEND(2), {0xE2,0x65}},
    {LCM_SEND(2), {0xE3,0xF8}},
    {LCM_SEND(1), {0x28}},
    {LCM_SLEEP(20)},
    {LCM_SEND(1), {0x10}},
    {LCM_SLEEP(120)},
    {LCM_SEND(2), {0xE0,0x00}},
    {LCM_SEND(2), {0xE1,0x09}},
    {LCM_SEND(2), {0xE2,0xB1}},
    {LCM_SEND(2), {0xE3,0x7F}},
};

static LCM_Init_Code sleep_out[] =  {
    {LCM_SEND(2), {0xE0,0x00}},
    {LCM_SEND(2), {0xE1,0x93}},
    {LCM_SEND(2), {0xE2,0x65}},
    {LCM_SEND(2), {0xE3,0xF8}},
    {LCM_SEND(1), {0x11}},
    {LCM_SLEEP(120)},
    {LCM_SEND(1), {0x29}},
    {LCM_SLEEP(20)},
    {LCM_SEND(2), {0xE0,0x00}},
    {LCM_SEND(2), {0xE1,0x09}},
    {LCM_SEND(2), {0xE2,0xB1}},
    {LCM_SEND(2), {0xE3,0x7F}},
};

static LCM_Force_Cmd_Code rd_prep_code[]={
	{0x37, {LCM_SEND(2), {0x3, 0}}},
};

static LCM_Force_Cmd_Code rd_prep_code_1[]={
	{0x37, {LCM_SEND(2), {0x1, 0}}},
};
static int32_t jd9365_mipi_init(struct panel_spec *self)
{
	int32_t i = 0;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

    mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
    mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;
	LCD_PRINT(KERN_DEBUG "jd9365_mipi_init\n");

	mipi_set_cmd_mode();

	for(i = 0; i < ARRAY_SIZE(init_data); i++){
		tag = (init->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
			udelay(20);
		}else if(tag & LCM_TAG_SLEEP){
			msleep((init->tag & LCM_TAG_MASK));
		}
		init++;
	}
	LCD_PRINT(KERN_DEBUG "jd9365_mipi_init kernel end \n");
	return 0;
}

static uint32_t jd9365_readid(struct panel_spec *self)
{
/*Jessica TODO: need read id*/
	int32_t i = 0;
	uint32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code;
	uint8_t read_data[3] = {0};
	
	char id_high=0;
	char id_midd=0;
	char id_low=0;
	int id=0;
	
	int32_t read_rtn = 0;
	unsigned int tag = 0;
	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
    mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
    mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	LCD_PRINT("lcd_jd9365_mipi kernel read id!\n");
	mipi_set_lp_mode();
	//mipi_set_cmd_mode();
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}else if(tag & LCM_TAG_SLEEP){
				mdelay((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}

        read_rtn = mipi_force_read(0x04, 2,(uint8_t *)read_data);
         id =  ((read_data[0]&0x00FF)<<8)|((read_data[1]&0x00FF));
		
		//printf("lcd_jd9365_mipi u-boot  jd9365_readid : (lcd_id === 0x%x,0x%x, 0x%x, !)\n", read_data[0] , read_data[1] , id );

		if((0x9365 == id || read_data[0] ==0x93 || read_data[1] ==0x65)){
		LCD_PRINT("lcd_jd9365_mipi read id success!\n");									///////////////////////0x06
		return 0x9161;
		}	
	}		
	mipi_set_hs_mode();
	LCD_PRINT("lcd_jd9365_mipi read id error!\n");
	//printf("lcd_jd9365_mipi u-boot  jd9365_readid : (lcd_id === 0x%x,0x%x, 0x%x, 0x%x!)\n", id_high , id_midd , id_low, (((id_high&0x00FF)<<8)|((id_midd&0x00FF))));
	return 0;
}

static int32_t jd9365_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
	int32_t i = 0;
	LCM_Init_Code *sleep_in_out = NULL;
	unsigned int tag;
	int32_t size = 0;

	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

	LCD_PRINT( "jd9365_enter_sleep,  is_sleep = %d\n", is_sleep);

	if(is_sleep){
		sleep_in_out = sleep_in;
		size = ARRAY_SIZE(sleep_in);
	}else{
		sleep_in_out = sleep_out;
		size = ARRAY_SIZE(sleep_out);
	}

	for(i = 0; i <size ; i++){
		tag = (sleep_in_out->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(sleep_in_out->data, (sleep_in_out->tag & LCM_TAG_MASK));
		}else if(tag & LCM_TAG_SLEEP){
			msleep((sleep_in_out->tag & LCM_TAG_MASK));
		}
		sleep_in_out++;
	}
	return 0;
}

static uint32_t jd9365_readpowermode(struct panel_spec *self)
{
	int32_t i = 0;
	uint32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code_1;
	uint8_t read_data[1] = {0};
	int32_t read_rtn = 0;
	unsigned int tag = 0;

	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	LCD_PRINT("lcd_jd9365_mipi read power mode!\n");
	mipi_eotp_set(0,1);
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code_1;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code_1); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}else if(tag & LCM_TAG_SLEEP){
				msleep((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}
		read_rtn = mipi_force_read(0x0A, 1,(uint8_t *)read_data);
		//LCD_PRINT("lcd_nt35516 mipi read power mode 0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
		if((0x9c == read_data[0])  && (0 == read_rtn)){
			LCD_PRINT("lcd_jd9365_mipi read power mode success!\n");
			mipi_eotp_set(1,1);
			return 0x9c;
		}
	}

	LCD_PRINT("lcd_jd9365 mipi read power mode fail!0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
	mipi_eotp_set(1,1);
	return 0x0;
}

static uint32_t jd9365_check_esd(struct panel_spec *self)
{
	uint32_t power_mode;

	mipi_set_lp_mode_t mipi_set_data_lp_mode = self->info.mipi->ops->mipi_set_data_lp_mode;
	mipi_set_hs_mode_t mipi_set_data_hs_mode = self->info.mipi->ops->mipi_set_data_hs_mode;
	mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
	mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	uint16_t work_mode = self->info.mipi->work_mode;

	LCD_PRINT("jd9365_check_esd!\n");
#ifndef FB_CHECK_ESD_IN_VFP
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_lp_mode();
	}else{
		mipi_set_data_lp_mode();
	}
#endif
	power_mode = jd9365_readpowermode(self);
	//power_mode = 0x0;
#ifndef FB_CHECK_ESD_IN_VFP
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_hs_mode();
	}else{
		mipi_set_data_hs_mode();
	}
#endif
	if(power_mode == 0x9c){
		LCD_PRINT("jd9365_check_esd OK!\n");
		return 1;
	}else{
		LCD_PRINT("jd9365_check_esd fail!(0x%x)\n", power_mode);
		return 0;
	}
}

static struct panel_operations lcd_jd9365_mipi_operations = {
	.panel_init = jd9365_mipi_init,
	.panel_readid = jd9365_readid,
	.panel_enter_sleep = jd9365_enter_sleep,
	.panel_esd_check = jd9365_check_esd,
};

static struct timing_rgb lcd_jd9365_mipi_timing = {
	.hfp = 20,  /* unit: pixel */
	.hbp = 20,
	.hsync = 20,//4,
	.vfp = 8, /*unit: line*/
	.vbp = 16,
	.vsync = 2,
};

static struct info_mipi lcd_jd9365_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width = 24, /*18,16*/
	.lan_number = 4,
	.phy_feq = 480*1000,
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_jd9365_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_jd9365_mipi_spec = {
	.width = 720,
	.height = 1280,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
	.is_clean_lcd = true,
	.info = {
		.mipi = &lcd_jd9365_mipi_info
	},
	.ops = &lcd_jd9365_mipi_operations,
};

struct panel_cfg lcd_jd9365_mipi = {
	/* this panel can only be main lcd */
	.dev_id = SPRDFB_MAINLCD_ID,
	.lcd_id = 0x9365,
	.lcd_name = "lcd_jd9365_mipi",
	.panel = &lcd_jd9365_mipi_spec,
};

static int __init lcd_jd9365_mipi_init(void)
{
	return sprdfb_panel_register(&lcd_jd9365_mipi);
}

subsys_initcall(lcd_jd9365_mipi_init);
