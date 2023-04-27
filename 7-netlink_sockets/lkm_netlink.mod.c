#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) =
{
  .name = KBUILD_MODNAME,
  .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
  .exit = cleanup_module,
#endif
  .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
    __used
__attribute__((section("__versions"))) =
{
  { 0x15b2dc7b, "module_layout" },
  { 0x71422714, "netlink_kernel_release" },
  { 0xbeafad2, "__netlink_kernel_create" },
  { 0x49a68917, "init_net" },
  { 0x4ea1ddf2, "netlink_unicast" },
  { 0xb6ed1e53, "strncpy" },
  { 0xe64cff2f, "__nlmsg_put" },
  { 0x7ec6b3e1, "__alloc_skb" },
  { 0xb81960ca, "snprintf" },
  { 0x2c9189e, "kmem_cache_alloc_trace" },
  { 0x68c70c2d, "kmalloc_caches" },
  { 0x50eedeb8, "printk" },
  { 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
  "depends=";


MODULE_INFO(srcversion, "38922B05566932671496EFC");
