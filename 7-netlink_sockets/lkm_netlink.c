
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

static struct sock *nl_sk = NULL;
static volatile int pid = 0;
static struct netlink_kernel_cfg cfg = {0};

/* returns 0 on success, -1 on error */
static int ipc_send_reply(struct nlmsghdr *nlh)
{
  struct sk_buff *skb_out = NULL;
  /* Get the incoming/existing Netlink payload */
  char *payload = nlmsg_data(nlh);
  unsigned int payload_len = nlh->nlmsg_len;
  /* Allocate kernel memory for our reply message */
  char *replymsg = (char *)kmalloc(512, GFP_ATOMIC);
  unsigned int replymsg_len = 0;
  /* Write our reply message to an internal buffer */
  replymsg_len = snprintf(replymsg, 512, "some reply message: %s\n",
                          "blahblahblah");
                          
  /* If snprintf() failed we need to abort */
  if (replymsg_len < 0)
  {
    return -1;
  }
  
  /* Allocate a new Netlink message
   * nlmsg_new() should return a non-null skb pointer */
  if (!(skb_out = nlmsg_new(replymsg_len, 0)))
  {
    /* Reply could not be built, abort */
    return -1;
  }
  
  /* Make enough room in the sending SKB for the reply payload */
  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, replymsg_len, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
  
  /* Copy reply to Netlink Data.  Expecting "dest" as return value */
  if (nlmsg_data(nlh) != strncpy(nlmsg_data(nlh), replymsg, replymsg_len))
  {
    /* Reply could not be built, abort */
    return -1;
  }
  
  /* nlmsg_unicast - unicast a netlink message
   * @sk: netlink socket to spread message to
   * @skb: netlink message as socket buffer
   * @portid: netlink portid of the destination socket
   * static inline int nlmsg_unicast(struct sock *, struct sk_buff *, u32)
   * @ret: returns <0 on error, will not return >0 according to net/netlink.
   */
  if (nlmsg_unicast(nl_sk, skb_out, pid) < 0)
  {
    return -1;
  }
  
  return 0;
}

/* Where Netlink will send us if a Netlink Message is received */
static void netlink_callback(struct sk_buff *skb)
{
  /* Extract the Netlink Message Header */
  struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;
  /* Process ID (PID) of sending process */
  pid = nlh->nlmsg_pid;
  /* Log the Netlink payload/data */
  printk(KERN_INFO "nl_payload=\"%s\"", (char *)nlmsg_data(nlh));
  
  /* Process the incoming data and build a reply string */
  if (ipc_send_reply(nlh))
  {
    /* Reply could not be built, abort */
    return;
  }
}

/* Module entry function */
static int __init ipc_init(void)
{
  /* Tell Netlink where our callback function is */
  cfg.input = netlink_callback;
  /* Create a Netlink socket */
  nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
  
  /* Make sure the NL socket was created successfully */
  if (nl_sk == NULL)
  {
    return -EINVAL;
  }
  
  return 0;
}

/* Module exit function */
static void __exit ipc_exit(void)
{
  /* Release the NL socket if it exists */
  if (nl_sk != NULL)
  {
    netlink_kernel_release(nl_sk);
  }
}

module_init(ipc_init);
module_exit(ipc_exit);

/* Module Info */
MODULE_AUTHOR("Vijay Manohar Dogiparthy <vijay.dogiparthy@intel.com>");
MODULE_DESCRIPTION("Netlink socket exercise");
MODULE_LICENSE("GPL");

