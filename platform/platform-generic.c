/* drivers/examples/hello_ldm.c
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
#include <linux/platform_device.h>

static int __devinit hello_probe(struct platform_device *pdev)
{
	printk("hello: %s entry\n", __func__);
	return 0;
}

static int __devexit hello_remove(struct platform_device *pdev)
{
	printk("hello: %s entry\n", __func__);
	return 0;
}

#ifdef CONFIG_PM
int hello_suspend(struct platform_device *pdev, pm_message_t state)
{

	return 0;
}

int hello_resume(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}
#else
#define hello_suspend NULL
#define hello_resume  NULL
#endif

static struct platform_driver hello_driver = {
	.driver = {
		.name = "hello",
		.owner = THIS_MODULE,
	},
	.probe   = hello_probe,
	.remove  = __devexit_p(hello_remove),
	.suspend = hello_suspend,
	.resume  = hello_resume,
};

static struct platform_device *hello_device;

static int __init hello_init(void)
{
	int ret;

	/* register the platform device */
	hello_device = platform_device_alloc("hello", -1);
	if (NULL == hello_device) {
		printk("hello: platform device allocation failed\n");
	}

	ret = platform_device_add(hello_device);
	if (ret < 0) {
		printk("hello: unable to add platform device\n");
		goto err_platform_device_add;
	}

	ret = platform_driver_register(&hello_driver);
	if (ret < 0) {
		printk("hello: unable to register platform driver\n");
		goto err_platform_driver_register;
	}

	printk("hello: driver initalized\n");
	return 0;

err_platform_device_add:
	platform_device_put(hello_device);
err_platform_driver_register:
	platform_device_unregister(hello_device);

	return ret;
}

static void __exit hello_exit(void)
{
	platform_driver_unregister(&hello_driver);
	platform_device_unregister(hello_device);
	printk("hello: driver exited\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_DESCRIPTION("example: Hello LDM");
MODULE_LICENSE("GPL");
