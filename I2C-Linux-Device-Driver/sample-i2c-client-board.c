/* arch/arm/mach-omap2/board-xxx.c
 *
 * Copyright (C) 2011 Dojip Kim.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that is will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABLILITY of FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Genernal Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/i2c.h>

/* you might add a irq or platform_data if need
 *
 * { I2C_BOARD_INFO("sample_i2c_client", 0x50),
 *   .irq = 100,
 *   .platform_data = &sample_i2c_2_pdata,
 * }
 */
static struct i2c_board_info i2c_2_sample_devs[] __initdata = {
	{ I2C_BOARD_INFO("sample_i2c_client", 0x50), },

};

int machine_init(void)
{
	i2c_register_board_info(2, i2c_2_sample_devs, 
			ARRAY_SIZE(i2c_2_sample_devs));

}
