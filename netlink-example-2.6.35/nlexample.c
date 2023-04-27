#include <linux/kernel.h>
#include <linux/module.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>

#ifndef NETLINK_EXAMPLE
#define NETLINK_EXAMPLE 21
#endif

#define NETLINK_USER 31
static struct netlink_kernel_cfg cfg = {0};


#define NLEX_GRP_MAX  0

static struct sock *nlsk;
static int myvar;

static void
nl_callback(struct sk_buff *skb)
{
  printk("the current value is %d\n", myvar);
}

static int __init nlexample_init(void)
{
#if 0
  nlsk = netlink_kernel_create(&init_net,
                               NETLINK_EXAMPLE,
                               NLEX_GRP_MAX,
                               nl_callback,
                               NULL,
                               THIS_MODULE);
#endif

  /* Tell Netlink where our callback function is */
  cfg.input = nl_callback;

  /* Create a Netlink socket */
  nlsk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

  if (nlsk == NULL)
  {
    printk(KERN_ERR "Can't create netlink\n");
    return -ENOMEM;
  }

  return 0;
}

void __exit nlexample_exit(void)
{
  netlink_kernel_release(nlsk);
}

module_init(nlexample_init);
module_exit(nlexample_exit);

MODULE_AUTHOR("Pablo Neira Ayuso <pablo@netfilter.org>");
MODULE_LICENSE("GPL");
