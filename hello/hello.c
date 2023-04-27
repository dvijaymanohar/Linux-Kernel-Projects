/* drivers/examples/hello_ldd.c
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
#include <linux/module.h>
#include <linux/init.h>

static int __init hello_init(void)
{
	printk("Hello LDD\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk("Bye LDD\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_DESCRIPTION("example: Hello LDD");
MODULE_LICENSE("GPL");
