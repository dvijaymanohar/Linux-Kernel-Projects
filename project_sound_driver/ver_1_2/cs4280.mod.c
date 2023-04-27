#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xfa02c634, "struct_module" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x48987892, "pci_unregister_driver" },
	{ 0x1b7d4074, "printk" },
	{ 0x23df7c20, "__pci_register_driver" },
	{ 0xb5145e00, "register_chrdev" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00001013d00006003sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "70D264E55D1ABB078C697D4");
