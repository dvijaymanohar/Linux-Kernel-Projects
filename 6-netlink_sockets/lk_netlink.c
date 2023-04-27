#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/netlink.h>
#include<linux/udp.h>

/* Optional Headers  not Required, If you don't want Remove*/
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/socket.h>
#include <linux/icmp.h>

/* These Headers are also Optional (Not Required),
   But these are Required for NETFILTER functionality */
#include <net/checksum.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/interrupt.h>

/* NetLink Definitions */
#define NETLINK_HEADER_SIZE     16
#define NETLINK_MUNISOCK        25
#define NL_MSG_TYPE             20
#define MAX_BUFFER_SIZE         1024

/* Driver Definitions */
#define DRIVER_AUTHOR      "VMD"
#define DRIVER_DESC        "SAMPLE_NETLINK_SOCKET"

#define MAX_DATA_SIZE      50

/* Dump Function */
static void dumpRecvData(unsigned char *data, unsigned int dataLen);

/* Declare Buffer */
unsigned char *ucBuffer = NULL;

/* Netlink Sock ID*/
static struct sock *nl_sock_id = NULL;

/* Process Id */
static pid_t pid = 0;

/*
 * Function Name : dumpRecvData()
 */
void dumpRecvData(unsigned char *data,  unsigned int len)
{
  unsigned int uIndx = 0;
  printk("The Data:\n");
  
  if (data)
  {
    for (uIndx = 0; uIndx < len; ++uIndx)
    {
      if (uIndx % 32 == 0)
      {
        printk("\n%4d:", uIndx);
      }
      
      if (uIndx % 4 == 0)
      {
        printk(" ");
      }
      
      printk("%02x", data[uIndx]);
    }
  }
  
  printk(" Length of Bytes: %d\n", len);
  printk("\n");
}

/*
 * Function Name : sendDataToUserSpace()
 */
void sendDataToUserSpace(unsigned char  *ucBuffer, int unDataLen)
{
  /* Intialise the Sk buffer */
  struct sk_buff *skb = NULL;
  /* Intialise the Netlink message Hdr */
  struct nlmsghdr *nlhdr = NULL;
  int nRet = 0;
  /* Allocate the Memory Using SKB for sending To Appl Space */
  skb = alloc_skb((unDataLen + sizeof(struct nlmsghdr)), GFP_ATOMIC);
  /* Move the Tail Pointer at end of the Buffer */
  skb_put(skb, (unDataLen + sizeof(struct nlmsghdr)));
  
  /* Validate the Skb */
  if (NULL != skb)
  {
    nlhdr = (struct nlmsghdr *)skb->data;
    nlhdr->nlmsg_len = NLMSG_SPACE(MAX_BUFFER_SIZE);
    /*pid=0 corresponds to kernel */
    nlhdr->nlmsg_pid = 0;
    nlhdr->nlmsg_flags = 0;
    nlhdr->nlmsg_seq = 1;
    nlhdr->nlmsg_type = NL_MSG_TYPE;
    /* Copy the Payload */
    memmove(NLMSG_DATA(nlhdr), ucBuffer, unDataLen);
    NETLINK_CB(skb).pid = 0;
    /*BSGATM.exe Process ID */
    NETLINK_CB(skb).dst_pid = pid ;
    NETLINK_CB(skb).dst_group = 0;  /* unicast */
    /* send the NetLink Message to Application space */
    nRet = netlink_unicast(nl_sock_id, skb, pid, MSG_DONTWAIT);
    
    if (nRet == -1)
    {
      printk("Not Send Successfully\n");
      kfree_skb(skb);
    }
    else
    {
      printk(" Successfully sent to Application space\n");
    }
  }
}


/*
 * Function Name : MsgFromNetLinkSock
 */
static void msgFromNetLinkSock(struct sock *sk, int len)
{
  struct sk_buff *skb = NULL;
  struct nlmsghdr *nlhdr = NULL;
  int type, unDataLen;
  
  /* skb_dequeue takes the  buffer from a queue.
   * Nothing(No Buffer) is there then it return  NULL pointer.*/
  while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL)
  {
    /* Extract the NetLink Header fields for Processing */
    nlhdr = (struct nlmsghdr *)skb->data;
    
    if (nlhdr->nlmsg_pid != 0)
    {
      pid = nlhdr->nlmsg_pid; /*pid of sending process */
    }
    
    /* Copy the Type of message */
    type = nlhdr->nlmsg_type;
    
    if (type != NL_MSG_TYPE)
    {
      printk(KERN_INFO "Recieved Message Type :%d", type);
      kfree_skb(skb);
      continue;
    }
    
    /* Point the data to Buffer */
    ucBuffer = NLMSG_DATA(nlhdr);
    /* Compute Actual Length */
    /* skb->len and nlhdr->nlmsg_len are same.*/
    //or unDataLen = nlhdr->nlmsg_len - NETLINK_HDR_SIZE;
    unDataLen = skb->len - NETLINK_HEADER_SIZE;
    /* dump Recieved Data*/
    dumpRecvData(ucBuffer, unDataLen);
    /* Send the Data to User SPace from Kernel */
    sendDataToUserSpace(ucBuffer, unDataLen);
    /* If this is not there, then it will throw an atomic error */
    kfree_skb(skb);
  }
}

/*
 * Function Name : netlinkProcess_init()
*/

static int __init init_netlinkAppl(void)
{
  printk(KERN_INFO "NFNL Loading netlinkProcess  Module\n");
  // Create NetLink Socket
  nl_sock_id = netlink_kernel_create(NETLINK_MUNISOCK,
                                     0,
                                     msgFromNetLinkSock,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
                                     NULL,
#endif
                                     THIS_MODULE);
                                     
  if (!nl_sock_id)
  {
    printk(KERN_ERR "NFNL %s: receive handler registration failed\n",
           __func__);
    return -ENOMEM;
  }
  
  return 0;
}
/*
* Function Name : cleanup Module()
*/

static void __exit exit_netlinkAppl(void)
{
  printk(KERN_INFO "NFNL UnLoading netlinkProcess  Module\n");
  
  if (nl_sock_id)
  {
    //netlink_kernel_release(nl_sock_id);
    sock_release(nl_sock_id->sk_socket);
  }
}

/* Module Init and exit */
module_init(init_netlinkAppl);
module_exit(exit_netlinkAppl);

/* Module properties */
MODULE_LICENSE("Proprietary");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("2.6.18.194");

