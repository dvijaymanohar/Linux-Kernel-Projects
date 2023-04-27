/*
** =============================================================================
**
**                              MODULE DESCRIPTION
**
** =============================================================================
*/

/* Host based test sys code */

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

#include <linux/fs.h>
#include <linux/cdev.h>

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

#define LK_NL_COMM_MAJOR 253
#define LK_NL_COMM_MINOR 0



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
  u8 pow_en;  /* 0x0 - Disable Pow 0x1 - Enable Pow */
} T_HCI_INTEL_FMR_SET_POW;

typedef struct
{
  u8 aud_en;  /* 0x0 - Disable aud 0x1 - Enable aud */
} T_HCI_INTEL_FMR_SET_AUD;

typedef union
{
  T_HCI_INTEL_FMR_WRITE   fmr_hci_write_cmd;
  T_HCI_INTEL_FMR_READ    fmr_hci_read_cmd;
  T_HCI_INTEL_FMR_SET_POW fmr_hci_set_pow;
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

static struct task_struct *lk_nl_send_task_pid = NULL;

/* Definition and Initialisation of wait queue */
static DECLARE_WAIT_QUEUE_HEAD(lk_nl_send_task_ev_wait);

static int lk_nl_send_task_wait_ev = TRUE;

static int lk_nl_send_task_die = FALSE;

/* Serialize the incoming messages from userspace. */
//static DEFINE_MUTEX(lk_nl_input_cmd_mutex);

static int lk_nl_flag = 0;

static struct semaphore lk_nl_sem;

static int  us_app_pid = 0;

static dev_t lk_nl_dev_nos;

static struct cdev *lk_nl_dev = NULL;
static int lk_nl_major_no = 0;
static int lk_nl_minor_no = 0;

static int lk_nl_no_of_devs = 1;

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

static void lk_nl_send_msg(void)
{
  struct nlmsghdr *nlh = NULL;
  struct sk_buff  *skb_out = NULL; /* skb to transfer the data to us app */
  int  rc = 0;

  fmr_hci_set_pow_cmd.cmd_hdr1.opcode = 1876;
  fmr_hci_set_pow_cmd.cmd_hdr2.param_tot_len = 2;

  /* socket buff allocation */
  skb_out = nlmsg_new(sizeof(T_FMR_HCI_CMD), GFP_ATOMIC);

  if (!skb_out)
  {
    printk(KERN_ERR "%s: Failed to allocate new skb\n", __func__);
    return -ENOMEM;
  }

  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, sizeof(T_FMR_HCI_CMD), 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  memcpy(nlmsg_data(nlh), &fmr_hci_set_pow_cmd, sizeof(T_FMR_HCI_CMD));

  rc = nlmsg_unicast(lk_nl_sock, skb_out, us_app_pid);

  if (rc < 0)
  {
    printk(KERN_EMERG "%s: Error while sending bak to user\n", __func__);
  }
}


/*
 * This function has to do the job on the data given by the userspace app and go
 * back to sleep.
 */
int lk_nl_send_task_fn(void *arg)
{
  (void) arg;

  //find need of daemonize("lk_nl_send_task_fn");, look for some code in the internet

  allow_signal(SIGKILL);
  set_current_state(TASK_INTERRUPTIBLE);

  while (!kthread_should_stop())
  {
    /* Sleep, untill the condition becomes true */
    wait_event(lk_nl_send_task_ev_wait,
               ((TRUE != lk_nl_send_task_wait_ev) || (TRUE == lk_nl_send_task_die)));

    lk_nl_send_task_wait_ev = TRUE;

    /* Do the job, whatever is meant here and go back to sleep */

    if (TRUE != lk_nl_send_task_die)
    {
      lk_nl_send_msg();
    }
  }

  printk(KERN_INFO "%s: LK NL Send task has been stopped !\r\n", __func__);

  return 0;
}


/*
 * This function ideally, awaken the lk_nl_send_task_fn in a serailised fashion
 */
void lk_nl_recv_fn(struct sk_buff *skb)
{
  //mutex_lock(&lk_nl_input_cmd_mutex);

  down_interruptible(&lk_nl_sem);

  printk(KERN_INFO "%s: Input func\n", __func__);

  if (NULL != skb)
  {
    /* reception part */
    struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;

    /* Storing the userspace app pid */
    us_app_pid = nlh->nlmsg_pid; /* us_app_pid of sending process */

    printk(KERN_INFO "%s: Netlink received msg payload:%s\n", __func__, (char *)nlmsg_data(nlh));

    lk_nl_send_task_wait_ev = FALSE;

    /* Wakeup the write task */
    wake_up(&lk_nl_send_task_ev_wait);
  }

  up(&lk_nl_sem);

  //mutex_unlock(&lk_nl_input_cmd_mutex);
}


static int lk_nl_dev_open(struct inode *inode, struct file *file)
{
  printk(KERN_EMERG "%s : %u\n", __func__, __LINE__);
	return 0;
}


static int lk_nl_dev_release(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG "%s : %u\n", __func__, __LINE__);
	return 0;
}


static const struct file_operations lk_nl_dev_fops = {
	.owner   = THIS_MODULE,
	.open    = lk_nl_dev_open,
	.release = lk_nl_dev_release
};


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
    printk("%s: sock already present\n", __func__);

    rc = 0; /* return success, as there nothing to report error */

    goto lk_nl_init_soc_err;
  }

  lk_nl_sock = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

  if (NULL == lk_nl_sock)
  {
    printk(KERN_ALERT "%s: Error creating socket.\n", __func__);
    rc = -ENOMEM;
    goto lk_nl_init_soc_err;
  }

  printk(KERN_INFO "%s: lk sock created %p\n", __func__, (void *)lk_nl_sock);

  /* Recv task creation */
  lk_nl_send_task_pid = kthread_run(lk_nl_send_task_fn, NULL, "Input from US");

  if (NULL == lk_nl_send_task_pid)
  {
    printk(KERN_ALERT "%s: Could not create recv task\n", __func__);
    rc = -EIO;
    goto lk_nl_init_recv_task_creatn_err;
  }

  printk(KERN_INFO "%s: lk recv thread created %p\n", __func__, (void *)lk_nl_send_task_pid);

  /* Char registration */

  /* dynamic */
  rc = alloc_chrdev_region(&lk_nl_dev_nos, 0, lk_nl_no_of_devs, "lk_nl_dev");

  if (rc < 0)
  {
    printk(KERN_ERR
           "lk_nl_comm: Couldn't alloc_chrdev_region, error : %d\n",
		       rc);
    goto lk_nl_init_recv_task_creatn_err;
  }

	lk_nl_major_no = MAJOR(lk_nl_dev_nos);
	lk_nl_minor_no = MINOR(lk_nl_dev_nos);
	printk(KERN_EMERG
         "lk_nl_comm: Dev Major no: %d Minor No: %d\n",
         lk_nl_major_no, lk_nl_minor_no);

  /* dynamic allocation */
  lk_nl_dev = cdev_alloc();
  cdev_init(lk_nl_dev, &lk_nl_dev_fops);
  lk_nl_dev->owner = THIS_MODULE;
  lk_nl_dev->ops   = &lk_nl_dev_fops;
  rc = cdev_add(lk_nl_dev, lk_nl_dev_nos, 1);

	if (rc)
  {
		printk(KERN_ERR
		       "lk_nl_comm: Couldn't cdev_add, error = %d\n", rc);
		return 1;
	}

  printk(KERN_INFO "lk_nl_comm: dev registered successfully\n");

  return rc;

lk_nl_init_chrdev_reg_err:
  lk_nl_send_task_die = TRUE;

  wake_up(&lk_nl_send_task_ev_wait);

  printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

  /* close the recv thread */
  kthread_stop(lk_nl_send_task_pid);

lk_nl_init_recv_task_creatn_err:
  netlink_kernel_release(lk_nl_sock);

lk_nl_init_soc_err:
  return rc;
}


static void __exit lk_nl_exit(void)
{
  down_interruptible(&lk_nl_sem);

  if (NULL != lk_nl_send_task_pid)
  {
    lk_nl_send_task_die = TRUE;

    wake_up(&lk_nl_send_task_ev_wait);

    printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

    /* close the recv thread */
    kthread_stop(lk_nl_send_task_pid);
  }

  printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

  if (NULL != lk_nl_sock)
  {
    printk("%s: releasing lk sock %p\n", __func__, (void *)lk_nl_sock);
    netlink_kernel_release(lk_nl_sock);
  }
  else
  {
    printk("%s: warning sock is NULL\n", __func__);
  }

  up(&lk_nl_sem);

  /* device de-registration */
  if (lk_nl_dev)
  {
    cdev_del(lk_nl_dev);
  }

  unregister_chrdev_region(lk_nl_dev_nos, lk_nl_no_of_devs);
}


module_init(lk_nl_init);
module_exit(lk_nl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");


