
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


#define NETLINK_USER 31
#define MAX_PAYLOAD  1024 /* maximum payload size*/

#define u32 unsigned int
#define u8  unsigned char
#define u16 unsigned short

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

struct nl_soc nl_socket = {0};

struct nl_mesg
{
  u32 index;
  u32 data_len;
  u8  *data;
};

int init_netlink(struct nl_soc *nl_socket)
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

void release_netlink(struct nl_soc *nl_socket)
{
  if (nl_socket->socket_fd)
  {
    close(nl_socket->socket_fd);
  }
}

int send_netlink(struct nl_soc  *nl_socket,
                 struct nl_mesg *nl_data,
                 u8  msg_type,
                 u16 msg_flags,
                 u32 msg_seq)
{
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_nl dst_addr;
  int payload_len = 0;
  int rc = 0;
  
  payload_len = nl_data ? sizeof(*nl_data) + nl_data->data_len : 0;
  //printf("%s : payload_len : %u sizeof(struct nlmsghdr) : %u\n", __FUNCTION__, payload_len, sizeof(*nlh));
  
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
    memcpy(NLMSG_DATA(nlh), nl_data->data, payload_len);
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

void sig_int_handler(int sig)
{
  printf("Signal %d recieved\n", sig);
  release_netlink(&nl_socket);
  exit(EXIT_SUCCESS);
}

int recv_netlink(struct nl_soc  *nl_socket,
                 struct nl_mesg **nl_data,
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
  nlh = (struct nlmsghdr *)realloc(nlh, buf_len);
  
  if (!nlh)
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
           "to spoof eCryptfs netlink packets\n",
           nladdr.nl_pid);
    goto recv_netlink_out;
  }
  
  pl_len = NLMSG_PAYLOAD(nlh, 0);
  
  if (pl_len)
  {
    *nl_data = malloc(sizeof(*nl_data));
    
    if (!*nl_data)
    {
      rc = -errno;
      printf("Failed to allocate memory for eCryptfs netlink message: %m\n");
      goto recv_netlink_free_out;
    }
    
    (*nl_data)->data = malloc(pl_len);
    
    if (NULL != (*nl_data)->data)
    {
      memcpy((*nl_data)->data, NLMSG_DATA(nlh), pl_len);
    }
    else
    {
      printf("(*nl_data)->data is NULL");
    }
  }
  
  *msg_seq  = nlh->nlmsg_seq;
  *msg_type = nlh->nlmsg_type;
  
recv_netlink_free_out:
  free(nlh);
  
recv_netlink_out:
  return rc;
}


int main()
{
  char mesg[50] = {0};
  int  rc = 0;
  int  msg_seq  = 0;
  u8   msg_type = 0;
  struct nl_mesg *msg_rx = NULL;
  struct nl_mesg msg_tx = {0};
  
  /* signal handler registration */
  signal(SIGINT, sig_int_handler);
  
#if 0
  sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
  
  if (-1 == sock_fd)
  {
    rc = -errno;
    printf("Failed to create the Netlink user sockets\n");
    goto main_out;
  }
  
  /* Source details */
  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid    = getpid(); /* self pid */
  src_addr.nl_groups = 0;
  rc = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
  
  if (rc)
  {
    rc = -errno;
    printf("Failed to bind to the netlink socket\n");
    goto main_out;
  }
  
  /* destination details */
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid    = 0; /* For Linux Kernel */
  dest_addr.nl_groups = 0; /* unicast */
  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  
  nlh->nlmsg_len   = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid   = getpid();
  nlh->nlmsg_flags = 0;
  strcpy(NLMSG_DATA(nlh), "Sending Vijay Manohar Dogiparthi");
  iov.iov_base = (void *)nlh;
  iov.iov_len  = nlh->nlmsg_len;
  memset(&msg_tx, 0, sizeof(msg_tx));
  msg_tx.msg_name    = (void *)&dest_addr; /* destination socket name */
  msg_tx.msg_namelen = sizeof(dest_addr);
  msg_tx.msg_iov     = &iov;
  msg_tx.msg_iovlen  = 1;
  printf("Sending message to kernel\n");
  
#if 1
  
  while (1)
  {
    printf("Enter a string here\n");
    fgets(mesg, 45, stdin);
    strcpy(NLMSG_DATA(nlh), mesg);
    sendmsg(sock_fd, &msg_tx, 0);
#if 1
    printf("Waiting for message from kernel\n");
    /* Read message from kernel */
    //memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    /* Read message from kernel */
    recvmsg(sock_fd, &msg_tx, 0);
    printf("\nReceived message payload: %s\n", (char *)NLMSG_DATA(nlh));
#endif
  }
  
#endif
  close(sock_fd);
#endif
  
  rc = init_netlink(&nl_socket);
  
  if (rc < 0)
  {
    printf("%s: error code : %d\n", __FUNCTION__, rc);
    goto main_out;
  }
  
  msg_tx.data = (u8 *) &mesg;
  msg_tx.index    = 0;
  
  if (NULL != msg_rx)
  {
    printf("%s: could not allocate mem for msg_rx\n", __FUNCTION__);
    goto main_out;
  }
  
  while (1)
  {
    printf("Enter a string here: ");
    fgets(mesg, 45, stdin);
    
    msg_tx.data_len = strlen(mesg);
    
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
    
    printf("Recieved message: %s\n\n", msg_rx->data);
    
    free(msg_rx->data);
    free(msg_rx);
  }
  
  
main_out:
  release_netlink(&nl_socket);
  return rc;
}

