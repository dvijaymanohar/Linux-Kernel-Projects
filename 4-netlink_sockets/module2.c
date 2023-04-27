
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <net/net_namespace.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");
MODULE_DESCRIPTION("Kernel Korner's working versinon of netlink sockets");

// Note: Debug is not implemented
static int debug = 0;

module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug information (default 0)");

static struct sock *nl_sk = NULL;

static void nl_data_ready(struct sock *sk, int len)
{
  wake_up_interruptible(sk->sk_sleep);
}

static void netlink_test()
{
  struct sk_buff *skb = NULL;
  struct nlmsghdr *nlh = NULL;
  int err = 0;
  u32 pid = 0;
  nl_sk = netlink_kernel_create(NETLINK_NITRO, nl_data_ready);
  skb = skb_recv_datagram(nl_sk, 0, 0, &err);
  nlh = (struct nlmsghdr *)skb->data;
  printk(KERN_INFO "%s: received netlink message payload: %s\n", __FUNCTION__,
         NLMSG_DATA(nlh));
  pid = nlh->nlmsg_pid;
  NETLINK_CB(skb).groups = 0;
  NETLINK_CB(skb).pid = 0;
  NETLINK_CB(skb).dst_pid = pid;
  NETLINK_CB(skb).dst_groups = 0;
  netlink_unicast(nl_sk, skb, pid, MSG_DONTWAIT);
  sock_release(nl_sk->sk_socket);
}

static int __init my_module_init(void)
{
  printk(KERN_INFO "Initializing Netlink Socket");
  netlink_test();
  return 0;
}

static void __exit my_module_exit(void)
{
  printk(KERN_INFO "Goodbye");
}

module_init(my_module_init);
module_exit(my_module_exit);

