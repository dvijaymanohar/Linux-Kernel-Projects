/*
 *  FM Radio Driver for Connectivity chip of Intel Corporation.
 *  This file provides interfaces to V4L2 subsystem.
 *
 *  This module interfaces with V4L2 subsystem and FM Radio Rx module
 *
 *  Copyright (c) 2013, Intel Corporation.
 *  Author: Vijay Manohar Dogiparthy <vijay.dogiparthy@intel.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


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

#include "lkm_nl_sys.h"
#include "lkm_nl_comm.h"


/*
** =============================================================================
**
**                              LOCAL DEFINES
**
** =============================================================================
*/

#define NETLINK_USER     31
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
  int pid;
  struct sk_buff *skb; /* skb to transfer the data to us app */
} T_LK_NL_DEV_REPLY;

/*
** =============================================================================
**
**                             LOCAL DATA DEFINITIONS
**
** =============================================================================
*/

static T_FMR_HCI_CMD fmr_hci_set_pow_cmd = {0};

struct sock *lk_nl_sock = NULL;

static struct task_struct *lk_nl_send_task_struct = NULL;

/* Definition and Initialisation of wait queue */
static DECLARE_WAIT_QUEUE_HEAD(lk_nl_send_task_ev_wait);

static int lk_nl_send_task_wait_ev = TRUE;

static int lk_nl_send_task_die = FALSE;

/* Serialize the incoming messages from userspace. */
//static DEFINE_MUTEX(lk_nl_input_cmd_mutex);

static int lk_nl_flag = 0;

static struct semaphore lk_nl_sem;

static dev_t lk_nl_comm_dev_nos;

static struct cdev *lk_nl_comm_dev = NULL;
static int lk_nl_major_no = 0;
static int lk_nl_minor_no = 0;

static int lk_nl_no_of_devs = 1;

//static int msgs_sent_num = 0; /*  Only for accounting purpose */

static T_LK_NL_DEV_REPLY lk_nl_comm_dev_reply;

T_FMR_HCI_CMD_COMPLETE_EV cmd_complete_ev_data = {0};

/*
** =============================================================================
**
**                            EXPORTED FUNCTIONS
**
** =============================================================================
*/

int fmr_hci_send_cmd(T_FMR_WRITE_CMD_BUFF *cmd_buff)
{
  struct nlmsghdr *nlh = NULL;
  int  rc = -ENOMEM;

  ASSERT(NULL != cmd_buff);

  /* socket buff allocation */
  lk_nl_comm_dev_reply.skb = nlmsg_new(sizeof(T_FMR_WRITE_CMD_BUFF), GFP_ATOMIC);
  if (NULL == lk_nl_comm_dev_reply.skb)
  {
    printk(KERN_ERR "%s: Failed to allocate new skb\n", __func__);
    goto fmr_hci_send_cmd_err;
  }

  nlh = nlmsg_put(lk_nl_comm_dev_reply.skb, 0, 0, NLMSG_DONE, sizeof(T_FMR_WRITE_CMD_BUFF), 0);
  if (NULL == nlh)
  {
    printk(KERN_EMERG "lk_nl_comm_dev: Error adding a message to SKB");
    rc = -EMSGSIZE;
    goto fmr_hci_send_cmd_free_skb;
  }

  NETLINK_CB(lk_nl_comm_dev_reply.skb).dst_group = 0; /* not in mcast group */

  memcpy(nlmsg_data(nlh), cmd_buff, sizeof(T_FMR_WRITE_CMD_BUFF));

  down_interruptible(&lk_nl_sem);

  if (FALSE == lk_nl_send_task_die)
  {
    rc = netlink_unicast(lk_nl_sock, lk_nl_comm_dev_reply.skb, lk_nl_comm_dev_reply.pid, 0);

    if (rc < 0)
	  {
	    printk(KERN_ERR "lk_nl_comm_dev: No userspace app at us_app_pid : %d\n", lk_nl_comm_dev_reply.pid);

	    printk(KERN_ERR "lk_nl_comm_dev: rc : %d\n", rc);
	  }
  }
  else
  {
    up(&lk_nl_sem);
    goto fmr_hci_send_cmd_free_skb;
  }

  up(&lk_nl_sem);

  return rc;

fmr_hci_send_cmd_free_skb:
  kfree_skb(lk_nl_comm_dev_reply.skb);
  lk_nl_comm_dev_reply.skb = NULL;

fmr_hci_send_cmd_err:
  return rc;
}


static bool reg_flag = FALSE;

long lk_nl_comm_dev_register(struct lk_nl_comm_proto *new_proto)
{
  long err = 0;
  unsigned long flags = 0;
  
  if (NULL == new_proto || NULL == new_proto->recv
      || NULL == new_proto->reg_complete_cb)
  {
    printk("new_proto/recv or reg_complete_cb not ready");
    return -EINVAL;
  }
  
  if (TRUE == reg_flag)
  {
    pr_err("FMR device already registered", );
    return -EALREADY;
  }
  



  /* can be from process context only */
  spin_lock_irqsave(&st_gdata->lock, flags);
  
  if (test_bit(ST_REG_IN_PROGRESS, &st_gdata->st_state))
  {
    pr_info(" ST_REG_IN_PROGRESS:%d ", new_proto->chnl_id);
    /* fw download in progress */
    
    add_channel_to_table(st_gdata, new_proto);
    st_gdata->protos_registered++;
    new_proto->write = st_write;
    
    set_bit(ST_REG_PENDING, &st_gdata->st_state);
    spin_unlock_irqrestore(&st_gdata->lock, flags);
    return -EINPROGRESS;
  }
  else if (st_gdata->protos_registered == ST_EMPTY)
  {
    pr_info(" chnl_id list empty :%d ", new_proto->chnl_id);
    set_bit(ST_REG_IN_PROGRESS, &st_gdata->st_state);
    st_recv = st_kim_recv;
    
    /* enable the ST LL - to set default chip state */
    st_ll_enable(st_gdata);
    
    /* release lock previously held - re-locked below */
    spin_unlock_irqrestore(&st_gdata->lock, flags);
    
    /* this may take a while to complete
     * since it involves BT fw download
     */
    err = st_kim_start(st_gdata->kim_data);
    
    if (err != 0)
    {
      clear_bit(ST_REG_IN_PROGRESS, &st_gdata->st_state);
      
      if ((st_gdata->protos_registered != ST_EMPTY) &&
          (test_bit(ST_REG_PENDING, &st_gdata->st_state)))
      {
        pr_err(" KIM failure complete callback ");
        st_reg_complete(st_gdata, err);
        clear_bit(ST_REG_PENDING, &st_gdata->st_state);
      }
      
      return -EINVAL;
    }
    
    spin_lock_irqsave(&st_gdata->lock, flags);
    
    clear_bit(ST_REG_IN_PROGRESS, &st_gdata->st_state);
    st_recv = st_int_recv;
    
    /* this is where all pending registration
     * are signalled to be complete by calling callback functions
     */
    if ((st_gdata->protos_registered != ST_EMPTY) &&
        (test_bit(ST_REG_PENDING, &st_gdata->st_state)))
    {
      pr_debug(" call reg complete callback ");
      st_reg_complete(st_gdata, 0);
    }
    
    clear_bit(ST_REG_PENDING, &st_gdata->st_state);
    
    /* check for already registered once more,
     * since the above check is old
     */
    if (st_gdata->is_registered[new_proto->chnl_id] == true)
    {
      pr_err(" proto %d already registered ",
             new_proto->chnl_id);
      spin_unlock_irqrestore(&st_gdata->lock, flags);
      return -EALREADY;
    }
    
    add_channel_to_table(st_gdata, new_proto);
    st_gdata->protos_registered++;
    new_proto->write = st_write;
    spin_unlock_irqrestore(&st_gdata->lock, flags);
    return err;
  }
  /* if fw is already downloaded & new stack registers protocol */
  else
  {
    add_channel_to_table(st_gdata, new_proto);
    st_gdata->protos_registered++;
    new_proto->write = st_write;
    
    /* lock already held before entering else */
    spin_unlock_irqrestore(&st_gdata->lock, flags);
    return err;
  }

  new_proto->write = st_write;

  
  pr_debug("done %s(%d) ", __func__, new_proto->chnl_id);
}
EXPORT_SYMBOL_GPL(lk_nl_comm_dev_register);


/*
** =============================================================================
**
**                       LOCAL FUNCTIONS DECLARATIONS
**
** =============================================================================
*/

#define FMR_BASE          0xB8000000
#define SINE_RX_MUTE_ADDR 0x7e02

static void lk_nl_send_msg(void)
{
  printk("lk_nl_comm_dev: Enable power: ");
  fmr_sys_power_enable(1);
  printk("\n");

  //printk("\nChecking: Mute func cmd data:");
  //fmr_sys_reg_write16(FMR_BASE + SINE_RX_MUTE_ADDR, TRUE);
  //printk("\n");

  printk("\nChecking: Audio Enable:");
  fmr_sys_setup_host_audio(TRUE, AUDIOOUT_DAC);
  printk("\n");
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
  lk_nl_send_task_die = FALSE;

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


static void lk_nl_comm_dev_handle_input(void)
{
  switch(cmd_complete_ev_data.cmd_ev_hdr2.params.fmr_hci_write_cmd_complete.cmd_opcode)
  {
    case HCI_INTEL_FMR_WRITE:
      printk("lk_nl_comm_dev: write cmd response recieved\n");

      if (FMR_HCI_CMD_EXEC_STATUS ==
          cmd_complete_ev_data.cmd_ev_hdr2.params.fmr_hci_write_cmd_complete.status)
      {
        printk("Write command exec successfully\n");
      }
      /* VMD Todo: Do the job, what ever is meant here */
      break;

    case HCI_INTEL_FMR_READ:
      printk("lk_nl_comm_dev: write cmd response recieved\n");
      /* VMD Todo: Do the job, what ever is meant here */
      break;

    case HCI_INTEL_FMR_SET_POWER:
      printk("lk_nl_comm_dev: write cmd response recieved\n");
      /* VMD Todo: Do the job, what ever is meant here */
      break;

    case HCI_INTEL_FMR_SET_AUDIO:
      printk("lk_nl_comm_dev: write cmd response recieved\n");
      /* VMD Todo: Do the job, what ever is meant here */
      break;

    case HCI_INTEL_FMR_IRQ_CONFIRM:
      printk("lk_nl_comm_dev: write cmd response recieved\n");
      /* VMD Todo: Do the job, what ever is meant here */
      break;

    default:
      printk("lk_nl_comm_dev: Invalid cmd response recieved\n");
      break;
   }
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

    lk_nl_comm_dev_reply.pid = nlh->nlmsg_pid; /* Storing the userspace app pid */

    memcpy(&cmd_complete_ev_data, nlmsg_data(nlh), sizeof(T_FMR_HCI_CMD_COMPLETE_EV));

    /* handle the input message from app */
    lk_nl_comm_dev_handle_input();

    lk_nl_send_task_wait_ev = FALSE;

    /* Wakeup the write task */
    wake_up(&lk_nl_send_task_ev_wait);
  }

  up(&lk_nl_sem);

  //mutex_unlock(&lk_nl_input_cmd_mutex);
}


static int lk_nl_comm_dev_open(struct inode *inode, struct file *file)
{
  int rc = 0;

  /* reply mesg info */
  lk_nl_comm_dev_reply.pid = 0;
  lk_nl_comm_dev_reply.skb = NULL;

  /* For kernel version >= 3.6 kernels */
  struct netlink_kernel_cfg cfg =
  {
    .input = lk_nl_recv_fn,
  };

  if (NULL != lk_nl_sock) /* Socket has been allocated already */
  {
    printk("%s: sock already present\n", __func__);

    rc = 0; /* return success, as there nothing to report error */

    goto lk_nl_comm_dev_open_soc_err;
  }

  lk_nl_sock = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

  if (NULL == lk_nl_sock)
  {
    printk(KERN_ALERT "%s: Error creating socket.\n", __func__);
    rc = -ENOMEM;
    goto lk_nl_comm_dev_open_soc_err;
  }

  printk(KERN_INFO "%s: lk sock created %p\n", __func__, (void *)lk_nl_sock);

  /* Recv task creation */
  lk_nl_send_task_struct = kthread_run(lk_nl_send_task_fn, NULL, "Input from US");

  if (NULL == lk_nl_send_task_struct)
  {
    printk(KERN_ALERT "%s: Could not create recv task\n", __func__);
    rc = -EIO;
    goto lk_nl_comm_dev_open_recv_task_creatn_err;
  }

  printk(KERN_INFO "%s: lk recv thread created %p\n", __func__, (void *)lk_nl_send_task_struct);

  return 0;

lk_nl_comm_dev_open_recv_task_creatn_err:
    netlink_kernel_release(lk_nl_sock);

lk_nl_comm_dev_open_soc_err:
    return rc;
}


static int lk_nl_comm_dev_release(struct inode *inode, struct file *file)
{
  int rc = 0;

  down_interruptible(&lk_nl_sem);

  if (NULL != lk_nl_send_task_struct)
  {
    lk_nl_send_task_die = TRUE;

    wake_up(&lk_nl_send_task_ev_wait);

    printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

    /* close the recv thread */
    kthread_stop(lk_nl_send_task_struct);

    lk_nl_send_task_struct = NULL;
  }

  printk(KERN_INFO "%s : %u\n", __func__, __LINE__);

  if (NULL != lk_nl_sock)
  {
    printk("%s: releasing lk sock %p\n", __func__, (void *)lk_nl_sock);

    netlink_kernel_release(lk_nl_sock);

    lk_nl_sock = NULL;
  }
  else
  {
    printk("%s: warning sock is NULL\n", __func__);
  }

  up(&lk_nl_sem);

  /* reply mesg info */
  lk_nl_comm_dev_reply.pid = 0;
  lk_nl_comm_dev_reply.skb = NULL;

  return rc;
}


static const struct file_operations lk_nl_comm_dev_fops = {
	.owner   = THIS_MODULE,
	.open    = lk_nl_comm_dev_open,
	.release = lk_nl_comm_dev_release
};


static int __init lk_nl_init(void)
{
  int rc = 0;

  sema_init(&lk_nl_sem, 1);

  /* Dynamic Char dev registration */
  rc = alloc_chrdev_region(&lk_nl_comm_dev_nos, 0, lk_nl_no_of_devs, "lk_nl_comm_dev");

  if (rc < 0)
  {
    printk(KERN_ERR
           "lk_nl_comm_dev: Couldn't alloc_chrdev_region, error : %d\n",
		       rc);
    goto lk_nl_init_chrdev_reg_err;
  }

	lk_nl_major_no = MAJOR(lk_nl_comm_dev_nos);
	lk_nl_minor_no = MINOR(lk_nl_comm_dev_nos);
	printk(KERN_EMERG
         "lk_nl_comm_dev: Dev Major no: %d Minor No: %d\n",
         lk_nl_major_no, lk_nl_minor_no);

  /* dynamic allocation */
  lk_nl_comm_dev = cdev_alloc();
  cdev_init(lk_nl_comm_dev, &lk_nl_comm_dev_fops);
  lk_nl_comm_dev->owner = THIS_MODULE;
  lk_nl_comm_dev->ops   = &lk_nl_comm_dev_fops;
  rc = cdev_add(lk_nl_comm_dev, lk_nl_comm_dev_nos, 1);

	if (rc)
  {
		printk(KERN_ERR
		       "lk_nl_comm_dev: Couldn't cdev_add, error = %d\n", rc);
		goto lk_nl_init_unreg_dev_no;
	}

  printk(KERN_INFO "lk_nl_comm_dev: dev registered successfully\n");


lk_nl_init_unreg_dev_no:
  unregister_chrdev_region(lk_nl_comm_dev_nos, lk_nl_no_of_devs);

lk_nl_init_chrdev_reg_err:
  return rc;
}


static void __exit lk_nl_exit(void)
{
  /* device de-registration */
  if (lk_nl_comm_dev)
  {
    cdev_del(lk_nl_comm_dev);
  }

  unregister_chrdev_region(lk_nl_comm_dev_nos, lk_nl_no_of_devs);
}


module_init(lk_nl_init);
module_exit(lk_nl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");


