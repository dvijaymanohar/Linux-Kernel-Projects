
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAYLOAD 1024
struct sockaddr_nl src_addr, dst_addr;
struct nlmsghdr *nlh = NULL;
struct msghdr msg;
struct iovec iov;
int sock_fd;

int main()
{
  sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_NITRO);
  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = getpid();
  src_addr.nl_groups = 0; // no multicast
  bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
  memset(&dst_addr, 0, sizeof(dst_addr));
  dst_addr.nl_family = AF_NETLINK;
  dst_addr.nl_pid = 0; // 0 means kernel
  dst_addr.nl_groups = 0; // no multicast
  nlh = (struct nlhmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  /* Fill the netlink message header */
  nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;
  strcpy(NLMSG_DATA(nlh), "Yoo-hoo, Mr. Kernel!");
  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  msg.msg_name = (void *)&dst_addr;
  msg.msg_namelen = sizeof(dst_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  sendmsg(sock_fd, &msg, 0);
  /* Read message from kernel */
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  recvmsg(sock_fd, &msg, 0);
  printf("Received message payload: %s\n", NLMSG_DATA(nlh));
  close(sock_fd);
  return (EXIT_SUCCESS);
}

