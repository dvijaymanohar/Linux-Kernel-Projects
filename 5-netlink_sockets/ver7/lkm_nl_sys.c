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
/* This file contains system related functions needed by the HLD/LLD. It includes
 * OS interfaces like Timer, Queue, IDI, Interrupt, etc.
 */

/*
** =============================================================================
**
**                              INCLUDE STATEMENTS
**
** =============================================================================
*/
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/firmware.h>


#include "aud_app_fmr_basic_types.h"

/*
** =============================================================================
**
**                               LOCAL DEFINES
**
** =============================================================================
*/
#define FMR_FW_DOWNLOAD_ADD (0x00)

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
**                               LOCAL DEFINITIONS
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
/* Store (little-endian) 16bits */
static FMR_BOOL fmr_sys_store_le16(WORD value, BYTE *data, u32 offset)
{
  if (NULL == data)
  {
    return FALSE;
  }

  data[offset + 1] = (value & 0xff00) >> 8;
  data[offset] 		 = value & 0x00ff;

  return TRUE;
}


/* Store (little-endian) 32bits */
static FMR_BOOL fmr_sys_store_le32(DWORD value, BYTE *data, u32 offset)
{
  if (NULL == data)
  {
    return FALSE;
  }

  data[offset + 3] = (value & 0xff000000) >> 24;
  data[offset + 2] = (value & 0xff0000) >> 16;
  data[offset + 1] = (value & 0xff00) >> 8;
  data[offset] 		 = value & 0x00ff;

  return TRUE;
}


/* Store (big-endian) 16bits */
static FMR_BOOL fmr_sys_store_be16(WORD value, BYTE *data, u32 offset)
{
  if (NULL == data)
  {
    return FALSE;
  }

  data[offset]     = (value & 0xff00) >> 8;
  data[offset + 1] = value & 0x00ff;

  return TRUE;
}


/* Store (big-endian) 32bits */
static FMR_BOOL fmr_sys_store_be32(DWORD value, BYTE *data, u32 offset)
{
  if (NULL == data)
  {
    return FALSE;
  }

  data[offset] 		 = (value & 0xff000000) >> 24;
  data[offset + 1] = (value & 0xff0000) >> 16;
  data[offset + 2] = (value & 0xff00) >> 8;
  data[offset + 3] = value & 0xff;

  return TRUE;
}


/* Read (big-endian) 16bits */
static WORD fmr_sys_read_be16(const BYTE *data, u32 offset)
{
  if (NULL == data)
  {
    return FALSE;
  }

  return (data[offset] << 8) + data[offset + 1];
}


/* Read (big-endian) 32bits */
static DWORD fmr_sys_read_be32(const BYTE *data, u32 offset)
{
  if (NULL == data)
  {
    return FALSE;
  }

  return ((DWORD)data[offset] << 24) + ((DWORD)data[offset + 1] << 16) +
          ((DWORD) data[offset + 2] << 8) + data[offset + 3];
}


static int fmr_hci_write_cmd_assembly(u8 *cmd_buff, u8 plen,
                                      T_FMR_HCI_CMD fmr_hci_cmd)
{
  int index = 0;

	/* Validate the input arguments */
  ASSERT(NULL != cmd_buff);
  ASSERT(0 != plen);
  ASSERT(fmr_hci_cmd.cmd_hdr2.param_tot_len >= FMR_HCI_WRITE_CMD_TOT_PARAM_MIN_LEN);
  ASSERT(fmr_hci_cmd.cmd_hdr2.param_tot_len <= FMR_HCI_WRITE_CMD_TOT_PARAM_MAX_LEN);

  /* Store the command header 2, Parameter Total Len field */
  cmd_buff[FMR_HCI_CMD_HDR2_TOT_LEN_POS] = fmr_hci_cmd.cmd_hdr2.param_tot_len;

  /* Store the command header 2, Address field */
  fmr_sys_store_le16(fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off,
                     cmd_buff, FMR_HCI_CMD_HDR2_ADDR_POS);

  switch (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode)
  {
    case FMR_HCI_ACCESS_MODE0:
    {
      if (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length % 1)
      {
        printk("Invalid Length or mode\n");
        return -1;
      }
    }
    break;

    case FMR_HCI_ACCESS_MODE1:
    {
      if (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length % 2)
      {
        printk("Invalid Length or mode\n");
        return -1;
      }
    }
    break;

    case FMR_HCI_ACCESS_MODE2:
    {
      if (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length % 4)
      {
        printk("Invalid Length or mode\n");
        return -1;
      }
    }
    break;

    default:
    {
      printk("Invalid Mode\n");
      return -1;
    }
    break;
  }

  /* Store the command header 2, Mode field */
  cmd_buff[FMR_HCI_CMD_HDR2_MODE_POS] = fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode;

  /* Store the command header 2, Length field */
  cmd_buff[FMR_HCI_CMD_HDR2_LEN_POS] = fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length;

  for (index = 0; index < fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length; index++)
  {
    cmd_buff[FMR_HCI_WRITE_CMD_DATA_POS + index] = fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.data[index];
  }

  return 0;
}


static int fmr_hci_read_cmd_assembly(u8 *cmd_buff, T_FMR_HCI_CMD fmr_hci_cmd)
{
  int rc = 0;

  /* Validate the input arguments */
  ASSERT(cmd_buff != NULL);
  ASSERT(fmr_hci_cmd.cmd_hdr2.param_tot_len == FMR_HCI_READ_CMD_TOT_PARAM_LEN);

  /* Store the command header 2, Parameter Total Len field */
  cmd_buff[FMR_HCI_CMD_HDR2_TOT_LEN_POS] = fmr_hci_cmd.cmd_hdr2.param_tot_len;

  /* Store the command header 2, Address field */
  fmr_sys_store_le16(fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off,
                     cmd_buff, FMR_HCI_CMD_HDR2_ADDR_POS);

  /* Store the command header 2, Mode field */
  cmd_buff[FMR_HCI_CMD_HDR2_MODE_POS] = fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode;

  /* Store the command header 2, Length field */
  cmd_buff[FMR_HCI_CMD_HDR2_LEN_POS] = fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length;

  /* Sanity check for mode using length field */
  switch (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode)
  {
    case FMR_HCI_ACCESS_MODE0:
    {
      if (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length % 1)
      {
        printk("Invalid Length or mode\n");
        rc = -1;
        goto fmr_hci_read_cmd_assembly_err;
      }
    }
    break;

    case FMR_HCI_ACCESS_MODE1:
    {
      if (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length % 2)
      {
        printk("Invalid Length or mode\n");
        rc = -1;
        goto fmr_hci_read_cmd_assembly_err;
      }
    }
    break;

    case FMR_HCI_ACCESS_MODE2:
    {
      if (fmr_hci_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length % 4)
      {
        printk("Invalid Length or mode\n");
        rc = -1;
        goto fmr_hci_read_cmd_assembly_err;
      }
    }
    break;

    default:
    {
      printk("Invalid Mode\n");
      rc = -1;
      goto fmr_hci_read_cmd_assembly_err;
    }
    break;
  }

fmr_hci_read_cmd_assembly_err:
  return rc;
}


static int fmr_hci_set_pow_cmd_assembly(u8 *cmd_buff, T_FMR_HCI_CMD fmr_hci_cmd)
{
  /* Validate the input arguments */
  ASSERT(NULL != cmd_buff);
  ASSERT(FMR_HCI_SET_POW_CMD_TOT_PARAM_LEN == fmr_hci_cmd.cmd_hdr2.param_tot_len);

  /* Store the command header 2, Parameter Total Len field */
  cmd_buff[FMR_HCI_CMD_HDR2_TOT_LEN_POS] = fmr_hci_cmd.cmd_hdr2.param_tot_len;

  /* Store the command header 2, Pow enable/disable */
  cmd_buff[FMR_HCI_CMD_HDR2_ADDR_POS] = fmr_hci_cmd.cmd_hdr2.params.fmr_hci_set_pow.pow_en;

  return 0;
}


static int fmr_hci_set_aud_cmd_assembly(u8 *cmd_buff, T_FMR_HCI_CMD fmr_hci_cmd)
{
  /* Validate the input arguments */
  ASSERT(NULL != cmd_buff);
  ASSERT(FMR_HCI_SET_AUD_CMD_TOT_PARAM_LEN == fmr_hci_cmd.cmd_hdr2.param_tot_len);

  /* Store the command header 2, Parameter Total Len field */
  cmd_buff[FMR_HCI_CMD_HDR2_TOT_LEN_POS] = fmr_hci_cmd.cmd_hdr2.param_tot_len;

  /* Store the command header 2, Pow enable/disable */
  cmd_buff[FMR_HCI_CMD_HDR2_ADDR_POS] =
  fmr_hci_cmd.cmd_hdr2.params.fmr_hci_set_aud.aud_en;

  return 0;
}


static int fmr_hci_confirm_irq_cmd_assembly(u8 *cmd_buff, T_FMR_HCI_CMD fmr_hci_cmd)
{
  /* Validate the input arguments */
  ASSERT(NULL != cmd_buff);
  ASSERT(FMR_HCI_CONFIRM_IRQ_CMD_TOT_PARAM_LEN == fmr_hci_cmd.cmd_hdr2.param_tot_len);

  /* Store the command header 2, Parameter Total Len field */
  cmd_buff[FMR_HCI_CMD_HDR2_TOT_LEN_POS] = fmr_hci_cmd.cmd_hdr2.param_tot_len;

  return 0;
}


static void fmr_hci_store_cmdhdr1(u8 *cmd_buff, T_FMR_HCI_CMD fmr_hci_cmd)
{
  /* Store the packet type */
  cmd_buff[FMR_HCI_CMD_PKT_TYPE_POS] = HCI_COMMAND_PKT;

  /* Store the command header 1 */
  fmr_sys_store_le16(fmr_hci_cmd.cmd_hdr1.opcode, cmd_buff,
                     FMR_HCI_CMD_HDR1_OP_POS);
}


static int fmr_hci_cmd_assembly(T_FMR_HCI_CMD fmr_hci_cmd,
                                u8 data_plen /* data length */ )
{
  int rc = 0;
  T_FMR_WRITE_CMD_BUFF cmd_buff = {0};

	lnp_fmdrv_dbg("%s: Cmd: %d\tdata_plen : %d\n", __FUNCTION__,
                 fmr_hci_cmd.cmd_hdr1.opcode, data_plen);

  fmr_hci_store_cmdhdr1(cmd_buff.cmd_buff, fmr_hci_cmd);

	switch(fmr_hci_cmd.cmd_hdr1.opcode)
  {
    case HCI_INTEL_FMR_WRITE:
    {
			rc = fmr_hci_write_cmd_assembly(cmd_buff.cmd_buff, data_plen, fmr_hci_cmd);
      cmd_buff.cmd_len = FMR_HCI_WRITE_CMD_LEN + data_plen;
    }
    break;

    case HCI_INTEL_FMR_READ:
    {
  		rc = fmr_hci_read_cmd_assembly(cmd_buff.cmd_buff, fmr_hci_cmd);
      cmd_buff.cmd_len = FMR_HCI_READ_CMD_LEN;
    }
    break;

    case HCI_INTEL_FMR_SET_POWER:
		{
			rc = fmr_hci_set_pow_cmd_assembly(cmd_buff.cmd_buff, fmr_hci_cmd);
      cmd_buff.cmd_len = FMR_HCI_SET_POW_CMD_LEN;
    }
    break;

    case HCI_INTEL_FMR_SET_AUDIO:
    {
			rc = fmr_hci_set_aud_cmd_assembly(cmd_buff.cmd_buff, fmr_hci_cmd);
      cmd_buff.cmd_len = FMR_HCI_SET_AUD_CMD_LEN;
    }
    break;

    case HCI_INTEL_FMR_IRQ_CONFIRM:
    {
      rc = fmr_hci_confirm_irq_cmd_assembly(cmd_buff.cmd_buff, fmr_hci_cmd);
      cmd_buff.cmd_len = FMR_HCI_CONFIRM_IRQ_CMD_LEN;
    }
    break;

    default:
    {
      rc = -1;
      printk("\n Recieved Cmd: 0x%X\n", fmr_hci_cmd.cmd_hdr1.opcode);
    }
    break;
  }


  fmr_hci_send_cmd(&cmd_buff);

  return rc;
}


static int fmr_sys_transfer_fw(int quanta, int size, FMR_UINT8 *src)
{
  u32 transferred = 0; 	/* no. of bytes transferred */
  u32 remaining = size; /* no. of bytes yet to be transferred */
  int rc = 0;

  if (size < quanta)
  {
    transferred = size;
    remaining = 0;
    lnp_fmdrv_dbg("\nTransferred : %d\tremaining over = %d\n", transferred, remaining);
  }
  else
  {
  	T_FMR_HCI_CMD fmr_hci_write_cmd;

    while (transferred != size)
    {
      if (remaining > quanta)
      {
        T_FMR_HCI_CMD fmr_hci_write_cmd;
        memset(&fmr_hci_write_cmd, 0, sizeof(T_FMR_HCI_CMD));

        fmr_hci_write_cmd.cmd_hdr1.opcode        = OPCODE(0x3F, 0x58);
        fmr_hci_write_cmd.cmd_hdr2.param_tot_len = 68;
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off = (FMR_FW_DOWNLOAD_ADD + transferred);
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode    = 0x2; 	/* 32 bit write access */
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length  = 0x40; /* no of bytes to write */
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.data    = src + transferred;

        printk(KERN_INFO "FMR Write Reg at addr: 0x%X Data len : %u",
                    fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off,
                    fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length);

        rc = fmr_hci_cmd_assembly(fmr_hci_write_cmd,
                                  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length);

        transferred += quanta;

				if (remaining > quanta)
        {
					remaining = remaining - quanta;
        }
        else
        {
          remaining = quanta - remaining;
        }
      }
      else
      {
        memset(&fmr_hci_write_cmd, 0, sizeof(T_FMR_HCI_CMD));

        fmr_hci_write_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x58);
        fmr_hci_write_cmd.cmd_hdr2.param_tot_len                    = FMR_HCI_WRITE_CMD_LEN + remaining;
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off = (0x0 + transferred);
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode    = 0x2;       /* 32 bit write access */
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length  = remaining; /* no of bytes to write */
        fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.data    = (src + transferred);

        printk(KERN_INFO "FMR Write Reg at addr: 0x%X Data len : %u",
                    fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off,
                    fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length);

        rc = fmr_hci_cmd_assembly(fmr_hci_write_cmd,
                             fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length);

        transferred += remaining;
        remaining = 0;
      }

      //lnp_fmdrv_dbg("Transfferred : %d\tremaining = %d\n", transferred, remaining);
    }
  }

  return rc;
}

/*
** =============================================================================
**
**                            EXPORTED FUNCTIONS
**
** =============================================================================
*/

int fmr_sys_reg_read16(FMR_UINT32 addr, FMR_UINT16 *data)
{
  T_FMR_HCI_CMD fmr_hci_read_cmd;
  int rc = 0;

	ASSERT(NULL != data);

  memset(&fmr_hci_read_cmd, 0, sizeof(T_FMR_HCI_CMD));

  fmr_hci_read_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x59);

  fmr_hci_read_cmd.cmd_hdr2.param_tot_len = FMR_HCI_READ_CMD_TOT_LEN;
  fmr_hci_read_cmd.cmd_hdr2.params.fmr_hci_read_cmd.add_off = LNP_ADDR_ACCESS(addr);
	fmr_hci_read_cmd.cmd_hdr2.params.fmr_hci_read_cmd.mode    = 0x1; /* 16 bit read access */
  fmr_hci_read_cmd.cmd_hdr2.params.fmr_hci_read_cmd.length  = 2;   /* no of bytes to read */

	rc = fmr_hci_cmd_assembly(fmr_hci_read_cmd, 0);

  lnp_fmdrv_dbg("%s: addr : 0x%p data : 0x%p\n\n",
                __FUNCTION__, (unsigned int)LNP_ADDR_ACCESS(addr),
                (unsigned int) *data);

  return rc;
}


int fmr_sys_reg_read32(FMR_UINT32 addr, FMR_UINT32 *data)
{
  T_FMR_HCI_CMD fmr_hci_read_cmd;
  int rc = 0;

  ASSERT(NULL != data);

  memset(&fmr_hci_read_cmd, 0, sizeof(T_FMR_HCI_CMD));

  fmr_hci_read_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x59);

  fmr_hci_read_cmd.cmd_hdr2.param_tot_len                    = FMR_HCI_READ_CMD_TOT_LEN;
  fmr_hci_read_cmd.cmd_hdr2.params.fmr_hci_read_cmd.add_off  = LNP_ADDR_ACCESS(addr);
	fmr_hci_read_cmd.cmd_hdr2.params.fmr_hci_read_cmd.mode     = 0x2; /* 32 bit read access */
  fmr_hci_read_cmd.cmd_hdr2.params.fmr_hci_read_cmd.length   = 4;   /* no of bytes to read */

  rc = fmr_hci_cmd_assembly(fmr_hci_read_cmd, 0);

	/* VMD Todo: How the data that should be given to the code, if required */

  lnp_fmdrv_dbg("%s  : addr : 0x%p data : 0x%p\n\n", __FUNCTION__,
                (unsigned int)LNP_ADDR_ACCESS(addr), (unsigned int) *data);

  return rc;
}


int fmr_sys_reg_write16(FMR_UINT32 addr, const FMR_UINT16 data)
{
  T_FMR_HCI_CMD fmr_hci_write_cmd;
  int rc = 0;

  memset(&fmr_hci_write_cmd, 0, sizeof(T_FMR_HCI_CMD));

	fmr_hci_write_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x58);

  fmr_hci_write_cmd.cmd_hdr2.param_tot_len = 6;

  /* Need to give, only the offset addr */
  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off = LNP_ADDR_ACCESS(addr);

  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode    = 0x1; /* 16 bit write access */
  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length  = 2;	 /* no of bytes to write */
  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.data    = (u8 *) &data;

  rc = fmr_hci_cmd_assembly(fmr_hci_write_cmd,
                            fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length);

  lnp_fmdrv_dbg("%s : addr : 0x%p data : 0x%p\n\n", __FUNCTION__,
    						(unsigned int)LNP_ADDR_ACCESS(addr), data);

  return rc;
}


int fmr_sys_reg_write32(FMR_UINT32 addr, const FMR_UINT32 data)
{
  T_FMR_HCI_CMD fmr_hci_write_cmd;
  u8 cmd_data[4] = {0};
  int rc = 0;

  memset(&fmr_hci_write_cmd, 0, sizeof(T_FMR_HCI_CMD));

  fmr_hci_write_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x58);

  fmr_hci_write_cmd.cmd_hdr2.param_tot_len = 8;

  /* Need to give, only the offset addr */
  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.add_off = LNP_ADDR_ACCESS(addr);
  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.mode    = 0x2; /* 4 byte write access */
  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length  = 4;   /* no of bytes to write */

  fmr_sys_store_le32(data, cmd_data, 0);

  fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.data    = (u8 *) cmd_data;

  rc = fmr_hci_cmd_assembly(fmr_hci_write_cmd,
                            fmr_hci_write_cmd.cmd_hdr2.params.fmr_hci_write_cmd.length);

  lnp_fmdrv_dbg("%s : addr : 0x%p data : 0x%X\n\n", __FUNCTION__,
    					  (unsigned int)LNP_ADDR_ACCESS(addr), data);

  return rc;
}


int fmr_sys_power_enable(FMR_BOOL enable)
{
	T_FMR_HCI_CMD fmr_hci_set_pow_cmd;
  int rc = 0;

	memset(&fmr_hci_set_pow_cmd, 0, sizeof(T_FMR_HCI_CMD));

	fmr_hci_set_pow_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x5A);

	fmr_hci_set_pow_cmd.cmd_hdr2.param_tot_len = 1;

  if (enable)
  {
    lnp_fmdrv_dbg("Power Enable: ");
		fmr_hci_set_pow_cmd.cmd_hdr2.params.fmr_hci_set_pow.pow_en = 0x1; /* enable pow */
  }
  else
  {
    lnp_fmdrv_dbg("Power Disable:");
  	fmr_hci_set_pow_cmd.cmd_hdr2.params.fmr_hci_set_pow.pow_en = 0x0; /* disable pow */
  }

	rc = fmr_hci_cmd_assembly(fmr_hci_set_pow_cmd, 0);

  return rc;
}


int fmr_sys_setup_host_audio(FMR_BOOL fmrx_aud_en, T_FMRX_AUD_ROUTING_SEL routing)
{
  T_FMR_HCI_CMD fmr_hci_set_aud_cmd;
  int rc = 0;

  memset(&fmr_hci_set_aud_cmd, 0, sizeof(T_FMR_HCI_CMD));

	ASSERT(AUDIOOUT_DAC == routing);

  fmr_hci_set_aud_cmd.cmd_hdr1.opcode = OPCODE(0x3F, 0x5B);
  fmr_hci_set_aud_cmd.cmd_hdr2.param_tot_len = FMR_HCI_SET_AUD_CMD_TOT_LEN;

	if (TRUE == fmrx_aud_en)
  {
  	fmr_hci_set_aud_cmd.cmd_hdr2.params.fmr_hci_set_aud.aud_en = 0x1; /* enable aud */
    lnp_fmdrv_dbg("%s: Audio enabled\n", __FUNCTION__);
  }
  else
  {
    fmr_hci_set_aud_cmd.cmd_hdr2.params.fmr_hci_set_aud.aud_en = 0x0; /* disable aud */
    lnp_fmdrv_dbg("%s: Audio Disabled\n", __FUNCTION__);
  }

  lnp_fmdrv_dbg("FMR Set Audio enable : 0x%p",
                fmr_hci_set_aud_cmd.cmd_hdr2.params.fmr_hci_set_aud.aud_en);

  rc = fmr_hci_cmd_assembly(fmr_hci_set_aud_cmd, 0);

  return rc;
}


void fmr_sys_busy_wait(FMR_UINT32 us)
{
  udelay(us);
}


int fmr_sys_mem_write(FMR_UINT8 *dst, FMR_UINT8 *src, FMR_UINT32 size)
{
  lnp_fmdrv_dbg(KERN_INFO "FM Radio Sine Rx FW Download\n\n");
	return fmr_sys_transfer_fw(64, size, src);
}

