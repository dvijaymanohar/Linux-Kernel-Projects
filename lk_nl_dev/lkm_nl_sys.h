
#ifndef LK_NL_SYS_H
#define LK_NL_SYS_H


#ifndef FMR_CHAR
  #define FMR_CHAR      char
#endif

#ifndef FMR_INT8
  #define FMR_INT8      signed char
#endif

#ifndef FMR_INT16
  #define FMR_INT16     signed short
#endif

#ifndef FMR_INT32
  #define FMR_INT32     signed int
#endif

#ifndef FMR_UINT8
  #define FMR_UINT8     unsigned char
#endif

#ifndef FMR_UINT16
  #define FMR_UINT16    unsigned short
#endif

#ifndef FMR_UINT32
  #define FMR_UINT32    unsigned int
#endif

#ifndef FMR_BOOL
  #define FMR_BOOL      signed int  /** As defined by SDHB */
#endif

#ifndef TRUE
  #define TRUE  1
#endif
#ifndef FALSE
  #define FALSE 0
#endif

#ifndef FMR_INT64
  #define FMR_INT64     long long
#endif

#ifndef FMR_UINT64
  #define FMR_UINT64    unsigned long long
#endif

#define BYTE  unsigned char
#define WORD  unsigned short
#define DWORD unsigned int

#define u8    unsigned char
#define u16   unsigned short
#define u32   unsigned int

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef FMR_BOOL
#define FMR_BOOL    signed int  /** As defined by SDHB */
#endif

#define FMR_BASE 0xB8000000
//#define FMR_FW_SIZE 32764

#define FMR_HCI_CMD_HDR_1_LEN sizeof(T_FMR_HCI_CMD_HDR_1)
#define FMR_HCI_CMD_HDR_2_LEN sizeof(T_FMR_HCI_CMD_HDR_2)

#define FMR_HCI_WRITE_CMD_LEN 			 7
#define FMR_HCI_READ_CMD_LEN  			 7
#define FMR_HCI_SET_POW_CMD_LEN  		 4
#define FMR_HCI_SET_AUD_CMD_LEN  		 4
#define FMR_HCI_CONFIRM_IRQ_CMD_LEN  3

#define FMR_HCI_READ_CMD_TOT_LEN  	 4
#define FMR_HCI_SET_AUD_CMD_TOT_LEN  1

/* FMR Command packet length */

#define FMR_HCI_WRITE_CMD_TOT_PARAM_MIN_LEN 		4
#define FMR_HCI_WRITE_CMD_TOT_PARAM_MAX_LEN 		68

#define FMR_HCI_READ_CMD_TOT_PARAM_LEN 					4
#define FMR_HCI_SET_POW_CMD_TOT_PARAM_LEN   		1
#define FMR_HCI_SET_AUD_CMD_TOT_PARAM_LEN  		  1
#define FMR_HCI_CONFIRM_IRQ_CMD_TOT_PARAM_LEN   0

#define FMR_HCI_CMD_PKT_TYPE_POS     						0
#define FMR_HCI_CMD_HDR1_OP_POS      						1

#define FMR_HCI_CMD_HDR2_TOT_LEN_POS (FMR_HCI_CMD_HDR1_OP_POS      + sizeof(u16))
#define FMR_HCI_CMD_HDR2_ADDR_POS    (FMR_HCI_CMD_HDR2_TOT_LEN_POS + sizeof(u8))
#define FMR_HCI_CMD_HDR2_MODE_POS    (FMR_HCI_CMD_HDR2_ADDR_POS 	 + sizeof(u16))
#define FMR_HCI_CMD_HDR2_LEN_POS     (FMR_HCI_CMD_HDR2_MODE_POS 	 + sizeof(u8))
#define FMR_HCI_WRITE_CMD_DATA_POS   (FMR_HCI_CMD_HDR2_LEN_POS 		 + sizeof(u8))

/* Mode Value defines */
#define FMR_HCI_ACCESS_MODE0 0x0	/* 08 bit read access */
#define FMR_HCI_ACCESS_MODE1 0x1  /* 16 bit read access */
#define FMR_HCI_ACCESS_MODE2 0x2  /* 32 bit read access */

/**
 * Packet types
 */
#define HCI_COMMAND_PKT       0x01
#define HCI_EVENT_PACKET      0x04
#define FMR_HCI_EVENT_PACKET  0x0E

#define FMR_HCI_CMD_EXEC_STATUS 0x0

#define HCI_INTEL_FMR_WRITE                0xFC58  /* OGF: 0x3F, OCF: 0x0058 */
#define HCI_INTEL_FMR_READ                 0xFC59  /* OGF: 0X3F, OCF: 0x0059 */
#define HCI_INTEL_FMR_SET_POWER            0xFC5A  /* OGF: 0x3F, OCF: 0x005A */
#define HCI_INTEL_FMR_SET_AUDIO            0xFC5B  /* OGF: 0X3F, OCF: 0x005B */
#define HCI_INTEL_FMR_IRQ_CONFIRM          0xFC5C  /* OGF: 0x3F, OCF: 0x005C */

#if 0  /* BT cmds are here for testing the code */
#define HCI_Intel_OTP_Burn_BT_Data            0xFC9C /* OGF: 3FH, OCF: 009C */
#define HCI_Intel_OTP_Write_BT_Base_Address   0xFC9D /* OGF: 3FH, OCF: 009D */
#define HCI_Intel_OTP_Read_Si_Production_Data 0xFC9E /* OGF: 3FH, OCF: 009E */
#define HCI_Intel_SW_RF_Kill                  0xFC3F /* OGF: 3FH, OCF: 003F */
#endif

#define OGF_MASK            0xfc00                       // 6 bits
#define OCF_MASK            0x03ff                       // 10 bits  ~OGF_MASK
#define CMD2OGF(cmd)        ((cmd & OGF_MASK) >> 10)     // cmdommand to OGF
#define CMD2OCF(cmd)        (cmd & OCF_MASK)             // Command to OCF

#define BE2LE(val)          (val << 8) | (val >> 8 )
#define LSB_DATA(val)       (val & 0x00FF)
#define MSB_DATA(val)       ((val & 0xFF00) >> 8)
#define LNP_ADDR_ACCESS(x)  (x - 0xB8000000)

/* calculate combined ogf/ocf value */
#define OPCODE(ogf, ocf)    (ocf | ogf << 10)

#define XGOLD_FMRX_FW_NAME  "fmr_rxmain.bin"
#define XGOLD_FMTX_FW_NAME  "fmr_txmain.bin"

//#define DEBUG_FMR

//#ifdef DEBUG_FMR
#define ASSERT(x)  			\
	do {									\
		if (!(x)) {					\
			printk(KERN_EMERG "assertion failed %s: %d: %s\n",	\
			       __FILE__, __LINE__, #x);				\
			BUG();						\
		}								    \
	} while (0)
//#else
//  #define ASSERT(x) do {} while (0)
//#endif

#ifdef DEBUG_FMR
#define lnp_fmdrv_dbg(format, ...) \
  printk(KERN_INFO "lnp_fmdrv: " format, ## __VA_ARGS__)
#else
#define lnp_fmdrv_dbg(format, ...) \
  do {} while (0)
#endif


#define lnp_fmdrv_err(format, ...) \
  printf(format, ## __VA_ARGS__)


/*
** =============================================================================
**
**                               LOCAL DEFINITIONS
**
** =============================================================================
*/

/* HCI Command packet structures */
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


typedef struct
{
	u8 aud_en;  /* 0x0 - Disable aud 0x1 - Enable aud */
} T_HCI_INTEL_FMR_IRQ_CONFIRM;


typedef union
{
	T_HCI_INTEL_FMR_WRITE   fmr_hci_write_cmd;
	T_HCI_INTEL_FMR_READ    fmr_hci_read_cmd;
	T_HCI_INTEL_FMR_SET_POW fmr_hci_set_pow;
	T_HCI_INTEL_FMR_SET_AUD fmr_hci_set_aud;
} T_FMR_HCI_CMD_PARAMS;


/* FMR VS HCI Command Header 1 */
typedef struct
{
	u16 opcode;        /* OCF (10 bit) and OGF (6 bit) */
} T_FMR_HCI_CMD_HDR_1;


/* FMR VS HCI Command Header 2 */
typedef struct
{
	u8 param_tot_len;
	T_FMR_HCI_CMD_PARAMS params;
} T_FMR_HCI_CMD_HDR_2;


/* Compact FMR HCI Command packet description */
typedef struct
{
	T_FMR_HCI_CMD_HDR_1 cmd_hdr1;
	T_FMR_HCI_CMD_HDR_2 cmd_hdr2;
} T_FMR_HCI_CMD;

/**
 * @brief
 * For audio routing selection
 */
typedef enum
{
  AUDIOOUT_DAC      = 0,     ///< DAC interface selected
  AUDIOOUT_SRC_48   = 1,     ///< SRC (Teaklite) interface selected
  AUDIOOUT_INVALID
} T_FMRX_AUD_ROUTING_SEL;

#define FMR_MAX_CMD_BUFF_LEN 71

typedef struct
{
  u8 cmd_len;
  u8 cmd_buff[FMR_MAX_CMD_BUFF_LEN];
} T_FMR_WRITE_CMD_BUFF;


/* HCI Command event structure constructs */
typedef struct
{
  u8 event_code;  /* command complete event code */
} T_FMR_HCI_CMD_EV_HDR_1;


typedef struct
{
  u8  num_hci_cmd_packets; /* Number of available command buffers */
  u16 cmd_opcode;          /* Command Opcode OGF: 0x3F, OCF: 0x58  */
  u8  status;              /* Command execution status */
} T_HCI_INTEL_FMR_WRITE_CMD_COMPLETE;


typedef struct
{
  u8  num_hci_cmd_packets; /* Number of available command buffers */
  u16 cmd_opcode;          /* Command Opcode OGF: 0x3F, OCF: 0x58  */
  u8  status;              /* Command execution status */
  u16 address;             /* Address offset from where the data was read */
  u8  data[64];            /* Refer param total len, to know the no. of bytes to access */
} T_HCI_INTEL_FMR_READ_CMD_COMPLETE;


typedef T_HCI_INTEL_FMR_WRITE_CMD_COMPLETE T_HCI_INTEL_FMR_SET_POW_CMD_COMPLETE;
typedef T_HCI_INTEL_FMR_WRITE_CMD_COMPLETE T_HCI_INTEL_FMR_SET_AUD_CMD_COMPLETE;
typedef T_HCI_INTEL_FMR_WRITE_CMD_COMPLETE T_HCI_INTEL_FMR_IRQ_CONFIRM_CMD_COMPLETE;


typedef union
{
  T_HCI_INTEL_FMR_WRITE_CMD_COMPLETE       fmr_hci_write_cmd_complete;
  T_HCI_INTEL_FMR_READ_CMD_COMPLETE        fmr_hci_read_cmd_complete;
  T_HCI_INTEL_FMR_SET_POW_CMD_COMPLETE     fmr_hci_set_pow_cmd_complete;
  T_HCI_INTEL_FMR_SET_AUD_CMD_COMPLETE     fmr_hci_set_aud_cmd_complete;
  T_HCI_INTEL_FMR_IRQ_CONFIRM_CMD_COMPLETE fmr_hci_set_irq_cfrm_cmd_complete;
} T_FMR_HCI_CMD_EV_PARAMS;


/* FMR VS HCI Command Event Header 2 */
typedef struct
{
	u8 param_tot_len;                /* Total Length of Parameter */
	T_FMR_HCI_CMD_EV_PARAMS params;  /* Parameters */
} T_FMR_HCI_CMD_EV_HDR_2;


/* Compact FMR HCI Command complete event description */
typedef struct
{
	T_FMR_HCI_CMD_EV_HDR_1 cmd_ev_hdr1;  /* event header 1 */
	T_FMR_HCI_CMD_EV_HDR_2 cmd_ev_hdr2;  /* event header 2 */
} T_FMR_HCI_CMD_COMPLETE_EV;

#endif
