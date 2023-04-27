#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

#define VENDOR_ID	0x058F
#define PRODUCT_ID	0x6387

static struct usb_device_id id_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{},
};
MODULE_DEVICE_TABLE(usb, id_table);

static int usb_sysfs_probe(struct usb_interface *interface, 
			  const struct usb_device_id *id)
{
	dev_info(&interface->dev,
		 "USB FS Device now attached\n");
        return 0;
}

static void usb_sysfs_disconnect(struct usb_interface *interface)
{
	dev_info(&interface->dev,
		 "USB FS Device now disconnected\n");
	return;
}

static struct usb_driver usb_sysfs_driver = {
//	.owner = THIS_MODULE,
	.name  = "usb_sysfs",
	.probe = usb_sysfs_probe,
	.disconnect = usb_sysfs_disconnect,
	.id_table = id_table,
};

struct usb_sysfs {
	struct usb_device *udev;
	int temp;
};

static int __init usb_sysfs_init(void)
{
	printk(KERN_INFO " Registering usb_sysfs driver\n");
	return usb_register(&usb_sysfs_driver);
}

static void __exit usb_syfs_exit(void)
{
	usb_deregister(&usb_sysfs_driver);
	return;
}

module_init(usb_sysfs_init);
module_exit(usb_syfs_exit);

MODULE_AUTHOR("Padmanabha S");
MODULE_DESCRIPTION("USB SYFS");
MODULE_LICENSE("GPL");

