
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
#include <netlink/netlink.h>

#define CINQ_MSG_REG 0x11
#define CINQ_MSG_RD  0x12
#define CINQ_MSG_WR  0x13
#define NETLINK_USER 31

const char REG_SIGNAL = 'c';

struct sockaddr_nl sock_addr_nl;

int running = 0;

int main(int argc, char *argv[])
{
  struct nl_sock *sock;
  int err;
  char *data;
  sock = nl_socket_alloc();
  unsigned char *msg = NULL;
  char *str = "Vijay Manohar";

  if (!sock)
  {
    fprintf(stderr, "[Err] Unable to allocate netlink socket.\n");
    return -1;
  }

  msg = malloc(50);

  sock_addr_nl.nl_family = AF_NETLINK;
  sock_addr_nl.nl_groups = 0;
  sock_addr_nl.nl_pad    = 0;
  sock_addr_nl.nl_pid    = getpid();

  err = nl_connect(sock, NETLINK_USER);

  if (err < 0)
  {
    nl_perror(err, "Unable to connect socket");
    nl_socket_free(sock);
    return -2;
  }

  data = malloc(sizeof(char));

  strcpy(msg, "Vijay Manohar");

#if 1
  *data = REG_SIGNAL;
  err = nl_send_simple(sock, CINQ_MSG_REG, NLM_F_ECHO, data, sizeof(char));
  free(data);

  if (err < 0)
  {
    nl_perror(err, "Failed to send register char");
    goto exit;
  }
#endif

  running = 1;

  while (running)
  {
    //nl_recv(struct nl_sock * sk, struct sockaddr_nl * nla, unsigned char * * buf, struct ucred * * creds);

    //err = nl_recvmsgs_default(sock);
    //err = nl_recvmsgs(sock);

    err = nl_send_simple(sock, CINQ_MSG_REG, NLM_F_ECHO, str, (strlen(str) + 1));

    if (err < 0)
    {
      nl_perror(err, "nl_send_simple");
    }

    err = nl_recv(sock, &sock_addr_nl, &msg, NULL);

    if (err < 0)
    {
      nl_perror(err, "nl_recv");
    }

    printf("msg content: %s\n", msg);
  }

exit:
  nl_close(sock);
  nl_socket_free(sock);
  return 0;
}

