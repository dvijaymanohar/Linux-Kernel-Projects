
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD  1024 /* maximum payload size*/

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;
int sock_fd = 0;

static int ipc_send(char *payload)
{
  struct timeval timeout;
  int ret = 0;

  /* Grab a free socket */
  sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);

  /* Did we get a socket open? */
  if (sock_fd < 0)
  {
    printf("%s: sock_fd < 0\n", __func__);
    return -1;
  }

  /* Init the Netlink struct */
  memset(&src_addr, 0, sizeof(src_addr));

  /* Set up a Netlink using our own Process ID */
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid    = getpid();

  /* Bind the Netlink to our Socket */
  bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

  /* Init the destination Netlink struct */
  memset(&dest_addr, 0, sizeof(dest_addr));

  /* Set up the destination Netlink struct using unicast */
  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid    = 0;  /* For Linux Kernel */
  dest_addr.nl_groups = 0;  /* unicast */

  /* Allocate memory for the Netlink message struct */
  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  nlh->nlmsg_len   = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid   = getpid();
  nlh->nlmsg_flags = 0;

  /* Inject our IPC message */
  strcpy(NLMSG_DATA(nlh), payload);
  iov.iov_base    = (void *)nlh;
  iov.iov_len     = nlh->nlmsg_len;
  msg.msg_name    = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov     = &iov;
  msg.msg_iovlen  = 1;

  /* set out timeout to 5 seconds */
  timeout.tv_sec  = 5;
  timeout.tv_usec = 0;

  /* Set the receive socket timeout value */
  if (setsockopt(sock_fd, SOL_SOCKET,
                 SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
  {
    printf("%s: setsockopt() failed\n", __func__);
    return -1;
  }

  /* Set the send socket timeout value */
  if (setsockopt(sock_fd, SOL_SOCKET,
                 SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
  {
    printf("%s: setsockopt() failed\n", __func__);
    return -1;
  }

  /* Try to send the message to the server */
  if (sendmsg(sock_fd, &msg, 0) < 0)
  {
    /* Did the call timeout? */
    if (EAGAIN == errno ||
        EWOULDBLOCK == errno)
    {
      printf("%s: sendmsg() timed out\n", __func__);
    }
    else
    {
      printf("%s: sendmsg() failed [errno=%u]\n", __func__, errno);
    }
  }

  return ret;
}

int main(int argc, char *argv[])
{
  char payload[MAX_PAYLOAD];
  int ret = 0;
  /* Create the payload */
  ret = snprintf(payload,
                 MAX_PAYLOAD,
                 "Hello kernel, how are you?");

  /* Send message to the kernel */
  if (ipc_send(payload))
  {
    printf("[%s: ipc_send() failed\n", __func__);
    return -1;
  }

  /* Read message from the kernel */
  if (recvmsg(sock_fd, &msg, 0) < 0)
  {
    /* Did the call timeout? */
    if (EAGAIN == errno ||
        EWOULDBLOCK == errno)
    {
      printf("%s: recvmsg() timed out\n", __func__);
    }
    else
    {
      printf("%s: recvmsg() failed [errno = %u]\n", __func__, errno);
    }
  }
  else
    /* Show the user the received data */
  {
    printf("%s\n", NLMSG_DATA(nlh));
  }

  /* Cleanup the socket */
  close(sock_fd);
  return ret;
}

