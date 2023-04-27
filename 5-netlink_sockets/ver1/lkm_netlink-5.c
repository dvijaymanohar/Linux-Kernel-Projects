
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

struct sock *knetlink_sk = NULL;

/*
** =============================================================================
**
**                              LOCAL TYPES
**
** =============================================================================
*/

typedef struct
{
  u16 add_off;
  u8  mode;
  u8  length;
  u8  *data;
} T_HCI_INTEL_FMR_WRITE;

typedef struct
{
  u16 add_off;
  u8  mode;
  u8  length;
} T_HCI_INTEL_FMR_READ;

typedef struct
{
  u8 pow_en;	/* 0x0 - Disable Pow 0x1 - Enable Pow */
} T_HCI_INTEL_FMR_SET_POW;

typedef struct
{
  u8 aud_en;	/* 0x0 - Disable aud 0x1 - Enable aud */
} T_HCI_INTEL_FMR_SET_AUD;

typedef union
{
  T_HCI_INTEL_FMR_WRITE 	fmr_hci_write_cmd;
  T_HCI_INTEL_FMR_READ  	fmr_hci_read_cmd;
  T_HCI_INTEL_FMR_SET_POW	fmr_hci_set_pow;
  T_HCI_INTEL_FMR_SET_AUD fmr_hci_set_aud;
} T_FMR_HCI_CMD_PARAMS;

typedef struct
{
  u16 opcode;        /* OCF (10 bit) and OGF (6 bit) */
} T_FMR_HCI_CMD_HDR_1;

typedef struct
{
  u8 param_tot_len;  /* VMD Todo: mem write; param tot length */
  T_FMR_HCI_CMD_PARAMS params;
} T_FMR_HCI_CMD_HDR_2;

/** Compact FMR HCI Command packet description */
typedef struct
{
  T_FMR_HCI_CMD_HDR_1 cmd_hdr1;
  T_FMR_HCI_CMD_HDR_2 cmd_hdr2;
} T_FMR_HCI_CMD;

static T_FMR_HCI_CMD fmr_hci_set_pow_cmd = {0};

static void knetlink_recv_msg(struct sk_buff *skb)
{
  struct nlmsghdr *nlh = NULL;
  int  pid = 0;
  struct sk_buff *skb_out = NULL; /* skb to transfer the data to us app */
  int  msg_size = 0;
  char *msg = "Hello Vijay Manohar from LK";
  int  rc = 0;

  msg_size = strlen(msg);

  /* reception part */
  nlh = (struct nlmsghdr *)skb->data;

  printk(KERN_EMERG "Netlink received msg payload:%s", (char *)nlmsg_data(nlh));

  /* transmission part */
  pid = nlh->nlmsg_pid; /* pid of sending process */

  /* socket buff allocation */
  skb_out = nlmsg_new(msg_size, GFP_ATOMIC);

  if (!skb_out)
  {
    printk(KERN_ERR "Failed to allocate new skb\n");
    return -ENOMEM;
  }

  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  /* Copy reply to Netlink Data.  Expecting "dest" as return value */
  if (nlmsg_data(nlh) != strncpy(nlmsg_data(nlh), msg, msg_size))
  {
    printk(KERN_EMERG "Reply could not be built, abort");
    return -1;
  }

  rc = nlmsg_unicast(knetlink_sk, skb_out, pid);

  if (rc < 0)
  {
    printk(KERN_EMERG "Error while sending bak to user\n");
  }
}

static int __init knetlink_init(void)
{
  int rc = 0;

  /* For kernel version >= 3.6 kernels */
  struct netlink_kernel_cfg cfg =
  {
    .input = knetlink_recv_msg,
  };

  if (NULL != knetlink_sk)
  {
    printk("knetlink_init: sock already present\n");
    rc = 1;
    goto knetlink_init_soc_err;
  }

  knetlink_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

#if 0 /* For kernels version < 3.6 */
  knetlink_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0,
                                      knetlink_recv_msg, NULL, THIS_MODULE);
#endif

  if (NULL == knetlink_sk)
  {
    printk(KERN_ALERT "Error creating socket.\n");
    rc = -ENOMEM;
    goto knetlink_init_soc_err;
  }

  printk("%s: sock %p\n", __func__, (void *)knetlink_sk);

knetlink_init_soc_err:
  return rc;
}

static void __exit knetlink_exit(void)
{
  if (NULL != knetlink_sk)
  {
    printk("%s: release sock %p\n", __func__, (void *)knetlink_sk);
    netlink_kernel_release(knetlink_sk);
  }
  else
  {
    printk("knetlink_exit: warning sock is NULL\n");
  }

  printk(KERN_INFO "exiting hello module\n");
}

module_init(knetlink_init);
module_exit(knetlink_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");
