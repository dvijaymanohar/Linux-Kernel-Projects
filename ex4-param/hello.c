/*
 * hello.c
 *
 * Parameter passing to a driver module
 *
 * Copyright (C) 2005 Shakthi Kannan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

#define DRIVER_AUTHOR "Shakthi Kannan <shaks@shakthimaan.com>"
#define DRIVER_DESC   "A sample driver"

MODULE_LICENSE ("GPL");
MODULE_AUTHOR (DRIVER_AUTHOR);
MODULE_DESCRIPTION (DRIVER_DESC);
MODULE_SUPPORTED_DEVICE ("TestDevice");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int array[2]= {-1, -1};
static int arr_argc = 0;

module_param (myshort, short, 0000);
MODULE_PARM_DESC (myshort, "A short integer");

module_param (myint, int, 0000); 
MODULE_PARM_DESC (myint, "An integer");

module_param (mylong, long, 0000); 
MODULE_PARM_DESC (mylong, "A long integer");

module_param (mystring, charp, 0000);
MODULE_PARM_DESC (mystring, "A character string");

static int __init hello_2_init (void)
{
	int i;

	printk (KERN_INFO "myshort is a short integer: %hd\n", myshort);
	printk (KERN_INFO "myint is an integer: %d\n", myint);
	printk (KERN_INFO "mylong is a long integer: %ld\n", mylong);
	printk (KERN_INFO "mystring is a string: %s\n\n", mystring);

	return 0;
}

static void __exit hello_2_exit (void)
{
	printk (KERN_INFO "hello driver cleaned up\n");
}

module_init (hello_2_init);
module_exit (hello_2_exit);
