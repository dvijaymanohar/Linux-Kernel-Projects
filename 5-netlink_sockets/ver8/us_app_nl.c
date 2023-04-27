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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <linux/netlink.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "us_app_nl.h"

/*
** =============================================================================
**
**                              LOCAL DEFINES
**
** =============================================================================
*/


#define NETLINK_USER 31

#define u32 unsigned int
#define u8  unsigned char
#define u16 unsigned short

#define LK_NL_DEV_FILE "/dev/lk_nl_dev"

#define FMR_MAX_CMD_BUFF_LEN 71


/*
** =============================================================================
**
**                              LOCAL TYPES
**
** =============================================================================
*/

struct nl_msg_rx
{
  //u32 index;
  //u32 data_len;
  T_FMR_WRITE_CMD_BUFF cmd_data;
};

struct nl_msg_tx
{
  //u32 index;
  //u32 data_len;
  //u8  *data;
  T_FMR_HCI_CMD_COMPLETE_EV cmd_comlete_ev;
};

/*
** =============================================================================
**
**                             LOCAL DATA DEFINITIONS
**
** =============================================================================
*/

#if 0
struct sockaddr_nl src_addr, dest_addr;
int sock_fd = 0;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;
#endif

struct nl_soc
{
  int socket_fd;
};

static int lk_nl_dev_fd = 0;

static struct nl_soc nl_socket = {0};



/*
** =============================================================================
**
**                       LOCAL FUNCTIONS DECLARATIONS
**
** =============================================================================
*/

static int init_netlink(struct nl_soc *nl_socket)
{
  struct sockaddr_nl src_addr;
  int rc = 0;

  nl_socket->socket_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);

  if (-1 == nl_socket->socket_fd)
  {
    rc = -errno;
    printf("Failed to create the Netlink user sockets\n");
    goto init_netlink_out;
  }

  /* Source details */
  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid    = getpid(); /* self pid */
  src_addr.nl_groups = 0;
  rc = bind(nl_socket->socket_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

  if (rc)
  {
    rc = -errno;
    printf("Failed to bind to the netlink socket\n");
    goto init_netlink_out;
  }

  printf("Netlink socket was successfully initialized\n");

init_netlink_out:
  return rc;
}


static void release_netlink(struct nl_soc *nl_socket)
{
  if (nl_socket->socket_fd)
  {
    close(nl_socket->socket_fd);
  }
}


static int send_netlink(struct nl_soc    *nl_socket,
                        struct nl_msg_tx *nl_data,
			                  u8  msg_type,
       				          u16 msg_flags,
              			    u32 msg_seq)
{
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_nl dst_addr;
  int payload_len = 0;
  int rc = 0;

  ASSERT(NULL != nl_socket);
  ASSERT(NULL != nl_data);

  //payload_len = nl_data ? sizeof(*nl_data) + nl_data->data_len : 0;
  payload_len = sizeof(*nl_data);

  nlh = (struct nlmsghdr *)(malloc(NLMSG_SPACE(payload_len) + sizeof(struct nlmsghdr)));

  if (NULL == nlh)
  {
    printf("Failed to allocate memory for netlink header\n");
    rc = -ENOMEM;
    goto send_netlink_out;
  }

  /* destination details */
  memset(&dst_addr, 0, sizeof(dst_addr));
  dst_addr.nl_family = AF_NETLINK;
  dst_addr.nl_pid    = 0; /* for Linux Kernel */
  dst_addr.nl_groups = 0; /* unicast */

  /* netlink message header */
  nlh->nlmsg_len   = NLMSG_LENGTH(payload_len);
  nlh->nlmsg_seq   = msg_seq;
  nlh->nlmsg_pid   = getpid(); /* pid of the sending process */
  nlh->nlmsg_type  = msg_type;
  nlh->nlmsg_flags = msg_flags;

  if (payload_len)
  {
    //memcpy(NLMSG_DATA(nlh), nl_data->data, payload_len);
    memcpy(NLMSG_DATA(nlh), &nl_data->cmd_comlete_ev, payload_len);
  }

  rc = sendto(nl_socket->socket_fd, nlh, nlh->nlmsg_len, 0,
              (struct sockaddr *)&dst_addr, sizeof(dst_addr));

  if (rc < 0)
  {
    rc = -errno;
    printf("Failed to send eCryptfs netlink message\n");
    goto send_netlink_out;
  }

send_netlink_out:
  free(nlh);

  return rc;
}


static void sig_int_handler(int sig)
{
  printf("Signal %d recieved\n", sig);
  release_netlink(&nl_socket);
  close(lk_nl_dev_fd);
  exit(EXIT_SUCCESS);
}


static int recv_netlink(struct nl_soc  *nl_socket,
                        struct nl_msg_rx **nl_data,
                        int  *msg_seq,
                        u8   *msg_type)
{
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_nl nladdr = {0};
  socklen_t nladdr_len = sizeof(nladdr);
  int flags   = MSG_PEEK;
  int buf_len = sizeof(*nlh);
  int pl_len  = 0;
  int rc      = 0;

receive:
  nlh = (struct nlmsghdr *) realloc(nlh, buf_len);

  if (NULL == nlh)
  {
    rc = -errno;
    printf("Failed to allocate memory for netlink message: %m\n");
    goto recv_netlink_out;
  }

  rc = recvfrom(nl_socket->socket_fd, nlh, buf_len, flags,
                (struct sockaddr *)&nladdr, &nladdr_len);

  printf("%s: recvfrom rc : %d\n", __FUNCTION__, rc);

  if (rc < 0)
  {
    rc = -errno;
    printf("Failed to receive netlink header; errno = "
           "[%d]; errno msg = [%m]\n", errno);
    goto recv_netlink_free_out;
  }

  if (flags & MSG_PEEK)
  {
    buf_len = nlh->nlmsg_len;
    flags   &= ~MSG_PEEK;
    goto receive;
  }

  if (nladdr_len != sizeof(nladdr))
  {
    rc = -EPROTO;
    printf("Received invalid netlink message\n");
    goto recv_netlink_free_out;
  }

  if (nladdr.nl_pid)
  {
    rc = -ENOMSG;
    printf("Received netlink packet from a "
           "userspace application; pid [%d] may be trying "
           "to spoof fmr netlink packets\n",
           nladdr.nl_pid);
    goto recv_netlink_out;
  }

  pl_len = NLMSG_PAYLOAD(nlh, 0);

  printf("%s: pl_len : %d\n", __FUNCTION__, pl_len);

  if (pl_len)
  {
    *nl_data = malloc(sizeof(struct nl_msg_rx));

    if (!*nl_data)
    {
      rc = -errno;
      printf("Failed to allocate memory for eCryptfs netlink message: %m\n");
      goto recv_netlink_free_out;
    }

    memcpy(&((*nl_data)->cmd_data), NLMSG_DATA(nlh), pl_len);
  }

  *msg_seq  = nlh->nlmsg_seq;
  *msg_type = nlh->nlmsg_type;

recv_netlink_free_out:
  free(nlh);

recv_netlink_out:
  return rc;
}


static int recv_netlink2(struct nl_soc  *nl_socket,
                        struct nl_msg_rx **nl_data,
                        int  *msg_seq,
                        u8   *msg_type)
{
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_nl nladdr = {0};
  socklen_t nladdr_len = sizeof(nladdr);
  int flags   = MSG_PEEK;
  int buf_len = sizeof(*nlh);
  int pl_len  = 0;
  int rc      = 0;

receive:
  nlh = (struct nlmsghdr *) realloc(nlh, buf_len);

  if (NULL == nlh)
  {
    rc = -errno;
    printf("Failed to allocate memory for netlink message: %m\n");
    goto recv_netlink_out;
  }

  rc = recvfrom(nl_socket->socket_fd, nlh, buf_len, flags,
                (struct sockaddr *)&nladdr, &nladdr_len);

  if (rc < 0)
  {
    rc = -errno;
    printf("Failed to receive netlink header; errno = "
           "[%d]; errno msg = [%m]\n", errno);
    goto receive;
    //goto recv_netlink_free_out;
  }

  if (flags & MSG_PEEK)
  {
    buf_len = nlh->nlmsg_len;
    flags   &= ~MSG_PEEK;
    goto receive;
  }

  if (nladdr_len != sizeof(nladdr))
  {
    rc = -EPROTO;
    printf("Received invalid netlink message\n");
    goto recv_netlink_free_out;
  }

  if (nladdr.nl_pid)
  {
    rc = -ENOMSG;
    printf("Received netlink packet from a "
           "userspace application; pid [%d] may be trying "
           "to spoof fmr netlink packets\n",
           nladdr.nl_pid);
    goto recv_netlink_out;
  }

  pl_len = NLMSG_PAYLOAD(nlh, 0);

  printf("%s: pl_len : %d\n", __FUNCTION__, pl_len);

  if (pl_len)
  {
    *nl_data = malloc(sizeof(struct nl_msg_rx));

    if (!*nl_data)
    {
      rc = -errno;
      printf("Failed to allocate memory for netlink message: %m\n");
      goto recv_netlink_free_out;
    }

    memcpy(&((*nl_data)->cmd_data), NLMSG_DATA(nlh), pl_len);
  }

  *msg_seq  = nlh->nlmsg_seq;
  *msg_type = nlh->nlmsg_type;

recv_netlink_free_out:
  free(nlh);

recv_netlink_out:
  return rc;
}


int main(int argc, char *argv[])
{
  //char mesg[50] = {0};

  int  msg_seq  = 0;
  u8   msg_type = 0;

  struct nl_msg_rx *msg_rx = NULL;
  struct nl_msg_tx msg_tx;//  = {0};

  int i  = 0;
  int rc = 0;

  /* signal handler registration */
  signal(SIGINT, sig_int_handler);

  lk_nl_dev_fd = open(LK_NL_DEV_FILE, O_RDWR);

  if (-1 == lk_nl_dev_fd)
  {
    rc = -errno;
    printf("Unable to open the device file : %s\n", LK_NL_DEV_FILE);
    goto main_out;
  }

  rc = init_netlink(&nl_socket);

  if (rc < 0)
  {
    printf("%s: error code : %d\n", __FUNCTION__, rc);
    goto main_out;
  }

  //msg_tx.data  = (u8 *) &mesg;
  //msg_tx.index = 0;

  while (1)
  {
    printf("\nEnter a string here: ");
    getchar();
    //fgets(mesg, 45, stdin);

    //msg_tx.data_len = strlen(mesg);
    msg_tx.cmd_comlete_ev.cmd_ev_hdr1.event_code    = 0xE;

    msg_tx.cmd_comlete_ev.cmd_ev_hdr2.param_tot_len = 4;
    msg_tx.cmd_comlete_ev.cmd_ev_hdr2.params.fmr_hci_write_cmd_complete.num_hci_cmd_packets = 0; /* VMD Todo: ?? */
    msg_tx.cmd_comlete_ev.cmd_ev_hdr2.params.fmr_hci_write_cmd_complete.cmd_opcode = HCI_INTEL_FMR_WRITE;
    msg_tx.cmd_comlete_ev.cmd_ev_hdr2.params.fmr_hci_write_cmd_complete.status     = 0;

    rc = send_netlink(&nl_socket, &msg_tx, 0, 0, 0);

    if (rc < 0)
    {
      printf("%s: Error in msg transmission with error code : %d\n", __FUNCTION__, rc);
      goto main_out;
    }

    rc = recv_netlink(&nl_socket, &msg_rx, &msg_seq, &msg_type);

    if (rc < 0)
    {
      printf("%s: Error in msg reception with error code : %d \n", __FUNCTION__, rc);
      goto main_out;
    }

    printf("\nCommand Content: \n");
    if (NULL != msg_rx)
    {
		  for (i = 0; i <= msg_rx->cmd_data.cmd_len; i++)
		  {
		    if (msg_rx->cmd_data.cmd_buff[i] <= 0xF)
			  {
			    printf("0%X ", (unsigned char)msg_rx->cmd_data.cmd_buff[i]);
			  }
			  else
			  {
			    printf("%X ", (unsigned char)msg_rx->cmd_data.cmd_buff[i]);
			  }
		  }
	  }

    free(msg_rx);
  }

main_out:
  release_netlink(&nl_socket);
  return rc;
}

