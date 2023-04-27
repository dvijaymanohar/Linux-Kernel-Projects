#include <sound/driver.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/rawmidi.h>
#include <sound/opl3.h>
#include <sound/initval.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

#define MAJOR_NUM 14 /*250*/
#define PCI_VENDOR_OUR_ID /*0x1011 */ 0x1013
#define PCI_DEVICE_OUR_ID /*0x6001 */ 0x6003


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cirrus Logic Sound Fusion CS46XX");
MODULE_AUTHOR("Ganesh S, Anantvijay, Amar Singh, Mohsin Roowalla");

struct file_operations fops;
struct pci_dev *pci;


static  struct pci_device_id snd_cs4280_id[] = {
	{PCI_VENDOR_OUR_ID, PCI_DEVICE_OUR_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0, }, /* CS4280 */
   {0,},
};


MODULE_DEVICE_TABLE(pci, snd_cs4280_id);


int our_probe(struct pci_dev *pci,  const struct pci_device_id *snd_cs4280_id)
{
	return -1;	
}

void our_remove(struct pci_dev *pci)
{
	
}

static struct pci_driver pci_driver = {
	.name = "snd_cs4280",
	.id_table = snd_cs4280_id,
	.probe = our_probe,
	.remove = our_remove,
};




static int snd_card_init_module(void)
{
	unsigned int return_val = 1;

//	fops.write = snd_play;
	if(register_chrdev(MAJOR_NUM, "snd_cs4280", &fops) == -1)
		return -1;
	if((return_val = pci_register_driver(&pci_driver)) == 0)
		printk(KERN_ALERT "Wow, successfully loaded with pci_dev address %p\n", pci);
	else
		printk(KERN_ALERT "Oooooopppsss... and pci_dev address is %p\n", pci);
	return return_val;
}

void snd_card_exit_module(void)
{
	pci_unregister_driver(&pci_driver);
	unregister_chrdev(MAJOR_NUM, "snd_cs4280");
	
}

module_init(snd_card_init_module);
module_exit(snd_card_exit_module);
