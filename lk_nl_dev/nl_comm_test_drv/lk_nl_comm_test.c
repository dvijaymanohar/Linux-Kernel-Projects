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

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/wait.h> /* wait queues */

#include "../lkm_nl_comm.h"


/*
** =============================================================================
**
**                              LOCAL DEFINES
**
** =============================================================================
*/


/*
** =============================================================================
**
**                              LOCAL TYPES
**
** =============================================================================
*/


/*
** =============================================================================
**
**                             LOCAL DATA DEFINITIONS
**
** =============================================================================
*/

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




static int __init lk_nl_comm_test_init(void)
{
  printk(KERN_INFO "%s: init successfully.\n", __func__);

  return 0;
}


static void __exit lk_nl_comm_test_exit(void)
{
  printk(KERN_INFO "%s: de-init successfully.\n", __func__);
}


module_init(lk_nl_comm_test_init);
module_exit(lk_nl_comm_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");


