/*
** =============================================================================
**
**                              MODULE DESCRIPTION
**
** =============================================================================
*/

/* Host Based test code */

/*
** =============================================================================
**
**                              INCLUDE STATEMENTS
**
** =============================================================================
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/kthread.h>
#include <linux/wait.h> /* wait queues */
#include <linux/timer.h>
#include <linux/jiffies.h>

//#include <asm/semaphore.h>


/*
** =============================================================================
**
**                              LOCAL DECLARATIONS
**
** =============================================================================
*/

#define NETLINK_USER 31
#define TRUE  1
#define FALSE 0

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

/*
** =============================================================================
**
**                             LOCAL DATA DEFINITIONS
**
** =============================================================================
*/

static T_FMR_HCI_CMD fmr_hci_set_pow_cmd = {0};

struct sock *lk_nl_sock = NULL;

static struct task_struct *lk_nl_recv_task_pid = NULL;

static int lk_nl_write_task_pid = -1;

/* Definition and Initialisation of wait queue */
static DECLARE_WAIT_QUEUE_HEAD(lk_nl_recv_task_ev_wait);

static int lk_nl_recv_task_wait_ev = TRUE;

static int lk_nl_recv_task_die = FALSE;

/* socket buff from us */
static struct sk_buff *lk_nl_data_buff = NULL;

/* Serialize the incoming messages from userspace. */
//static DEFINE_MUTEX(lk_nl_input_cmd_mutex);

static int lk_nl_flag = 0;

static struct semaphore lk_nl_sem;

/*
** =============================================================================
**
**                            EXPORTED FUNCTIONS
**
** =============================================================================
*/

/*
** =============================================================================
**
**                       LOCAL FUNCTIONS DECLARATIONS
**
** =============================================================================
*/

#if 0
static int lk_nl_recv_msg2(void *arg)
{
  struct nlmsghdr *nlh = NULL;
  int  pid = 0;
  struct sk_buff *skb_out = NULL; /* skb to transfer the data to us app */
  int  msg_size = 0;
  char *msg = "Hello Vijay Manohar from LK";
  int  rc = 0;
  struct sk_buff *skb = (struct sk_buff *) arg;

  msg_size = strlen(msg);

  /* reception part */
  nlh = (struct nlmsghdr *)skb->data;

  printk(KERN_EMERG "Netlink received msg payload:%s", (char *)nlmsg_data(nlh));

  /* transmission part */
  pid = nlh->nlmsg_pid; /* pid of sending process */

  fmr_hci_set_pow_cmd.cmd_hdr1.opcode = 1876;
  fmr_hci_set_pow_cmd.cmd_hdr2.param_tot_len = 2;

  /* socket buff allocation */
  skb_out = nlmsg_new(sizeof(T_FMR_HCI_CMD), GFP_ATOMIC);

  if (!skb_out)
  {
    printk(KERN_ERR "Failed to allocate new skb\n");
    rc = -ENOMEM;
    goto lk_nl_recv_msg2_skb_out_err;
  }

  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, sizeof(T_FMR_HCI_CMD), 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  /* Copy reply to Netlink Data.  Expecting "dest" as return value */
#if 0
  if (nlmsg_data(nlh) != strncpy(nlmsg_data(nlh), msg, msg_size))
  {
    printk(KERN_EMERG "Reply could not be built, abort");
    return -1;
  }
#endif

  memcpy(nlmsg_data(nlh), &fmr_hci_set_pow_cmd, sizeof(T_FMR_HCI_CMD));

  //rc = nlmsg_unicast(lk_nl_sock, skb_out, pid);

  if (rc < 0)
  {
    printk(KERN_EMERG "Error while sending bak to user\n");
  }

lk_nl_recv_msg2_skb_out_err:
  lk_nl_flag = 1;

  return rc;
}


/*
 * This function ideally, awaken the lk_nl_recv_task_fn in a serailised fashion
 */
void lk_nl_recv_fn(struct sk_buff *skb)
{
  mutex_lock(&lk_nl_input_cmd_mutex);
  struct task_struct *lk_nl_recv_task_pid = NULL;

  printk(KERN_INFO "%s: Input func\n", __func__);

  /* Recv task creation */
  lk_nl_recv_task_pid = kthread_run(lk_nl_recv_msg2, skb, "lk_nl_input_fn");

	if (!IS_ERR(lk_nl_recv_task_pid))
  {
    printk(KERN_ALERT "Error running kthread.\n");
  }

  while (!lk_nl_flag);

	//kfree_skb(skb);
  mutex_unlock(&lk_nl_input_cmd_mutex);
}
#endif


static void lk_nl_recv_msg(struct sk_buff *skb)
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

  fmr_hci_set_pow_cmd.cmd_hdr1.opcode = 1876;
  fmr_hci_set_pow_cmd.cmd_hdr2.param_tot_len = 2;

  /* socket buff allocation */
  skb_out = nlmsg_new(sizeof(T_FMR_HCI_CMD), GFP_ATOMIC);

  if (!skb_out)
  {
    printk(KERN_ERR "Failed to allocate new skb\n");
    return -ENOMEM;
  }

  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, sizeof(T_FMR_HCI_CMD), 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  /* Copy reply to Netlink Data.  Expecting "dest" as return value */
#if 0
  if (nlmsg_data(nlh) != strncpy(nlmsg_data(nlh), msg, msg_size))
  {
    printk(KERN_EMERG "Reply could not be built, abort");
    return -1;
  }
#endif

  memcpy(nlmsg_data(nlh), &fmr_hci_set_pow_cmd, sizeof(T_FMR_HCI_CMD));

  rc = nlmsg_unicast(lk_nl_sock, skb_out, pid);

  if (rc < 0)
  {
    printk(KERN_EMERG "Error while sending bak to user\n");
  }
}


/*
 * This function has to do the job on the data given by the userspace app and go
 * back to sleep.
 */
int lk_nl_recv_task_fn(void *arg)
{
  (void) arg;

  //find need of daemonize("lk_nl_recv_task_fn");, look for some code in the internet

  allow_signal(SIGKILL);
  set_current_state(TASK_INTERRUPTIBLE);

	while (!kthread_should_stop())
	{
    /* Sleep, untill the condition becomes true */
    wait_event(lk_nl_recv_task_ev_wait,
               ((TRUE != lk_nl_recv_task_wait_ev) || (TRUE == lk_nl_recv_task_die)));

    lk_nl_recv_task_wait_ev = TRUE;

    /* Do the job, whatever is meant here and go back to sleep */

    /* Process the incoming packet */
    if (NULL != lk_nl_data_buff)
    {
      lk_nl_recv_msg(lk_nl_data_buff);
    }
	}

  printk(KERN_INFO "LK NL Recv Task Stopped !\r\n");

  return 0;
}


/*
 * This function ideally, awaken the lk_nl_recv_task_fn in a serailised fashion
 */
void lk_nl_recv_fn(struct sk_buff *skb)
{
  //mutex_lock(&lk_nl_input_cmd_mutex);

  /* Need another Sem lock to protect from us app invocation and module removal */

  down_interruptible(&lk_nl_sem);

  printk(KERN_INFO "%s: Input func\n", __func__);

  if (NULL != skb)
  {
    lk_nl_data_buff = skb;

    lk_nl_recv_task_wait_ev = FALSE;

    /* Wakeup the guys, on slumbur */
    wake_up(&lk_nl_recv_task_ev_wait);
  }

  up(&lk_nl_sem);

  //mutex_unlock(&lk_nl_input_cmd_mutex);
}


static int __init lk_nl_init(void)
{
  int rc = 0;

  sema_init(&lk_nl_sem, 1);

  /* For kernel version >= 3.6 kernels */
  struct netlink_kernel_cfg cfg =
  {
    .input = lk_nl_recv_fn,
  };

  if (NULL != lk_nl_sock) /* Socket has been allocated already */
  {
    printk("lk_nl_init: sock already present\n");

    rc = 0; /* return success, as there nothing to report error */

    goto lk_nl_init_soc_err;
  }

  lk_nl_sock = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

  if (NULL == lk_nl_sock)
  {
    printk(KERN_ALERT "Error creating socket.\n");
    rc = -ENOMEM;
    goto lk_nl_init_soc_err;
  }

  printk(KERN_INFO "%s: lk sock created %p\n", __func__, (void *)lk_nl_sock);

  /* Recv task creation */
  lk_nl_recv_task_pid = kthread_run(lk_nl_recv_task_fn, NULL, "Input from US");

  if (NULL == lk_nl_recv_task_pid)
  {
    printk(KERN_ALERT "Could not create recv task");
    rc = -EIO;
    goto lk_nl_init_recv_task_creatn_err;
  }

  printk(KERN_INFO "%s: lk recv thread created %p\n", __func__, (void *)lk_nl_recv_task_pid);

  return rc;

lk_nl_init_recv_task_creatn_err:
  netlink_kernel_release(lk_nl_sock);

lk_nl_init_soc_err:
  return rc;
}


static void __exit lk_nl_exit(void)
{
  down_interruptible(&lk_nl_sem);

  if (NULL != lk_nl_recv_task_pid)
  {
    lk_nl_data_buff = NULL;

    lk_nl_recv_task_die = TRUE;

    wake_up(&lk_nl_recv_task_ev_wait);

    printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

    /* close the recv thread */
    kthread_stop(lk_nl_recv_task_pid);
  }

  printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

  if (NULL != lk_nl_sock)
  {
    printk("%s: releasing lk sock %p\n", __func__, (void *)lk_nl_sock);
    netlink_kernel_release(lk_nl_sock);
  }
  else
  {
    printk("lk_nl_exit: warning sock is NULL\n");
  }

  printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

  up(&lk_nl_sem);
}


module_init(lk_nl_init);
module_exit(lk_nl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");
