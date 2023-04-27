#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>

/* Netlink Defines */
#define NL_MSG_TYPE         20
#define NETLINK_MUNISOCK    25
#define NETLINK_HEADER_SIZE 16
#define MAX_DATA_SIZE       30

/* Socket FD */
int netlinkSockFD;
struct sockaddr_nl SrcAddr;
struct sockaddr_nl DestAddr;

/* Sequence No */
int SeqNo;

/* process Id */
pid_t PID;

/* Configure The Structure */
unsigned char ucBuffer[MAX_DATA_SIZE];

bool sendDataToKernel(int len)
{
  unsigned char buf[1024] = {0};
  bool ret = false;
  int errno;
  unsigned int uIndx;

  /* Intialise the buffer */
  for (uIndx = 0; uIndx < len; uIndx++)
  {
    ucBuffer[uIndx] = 0x31;
  }

  struct nlmsghdr *nlh = (struct nlmsghdr *)buf;

  nlh->nlmsg_len = (len + NETLINK_HEADER_SIZE);

  nlh->nlmsg_pid = PID;  /* self pid */

  nlh->nlmsg_seq = SeqNo++;

  nlh->nlmsg_flags = 0;

  nlh->nlmsg_type = NL_MSG_TYPE;

  /* Copy the Buffer */
  memcpy(NLMSG_DATA(nlh), &ucBuffer, len);

  /* Send the Data */
  int res = sendto(netlinkSockFD, nlh, nlh->nlmsg_len, 0,
                   (const struct sockaddr *) &DestAddr, sizeof(struct
                       sockaddr_nl));

  if (-1 == res)
  {
    cout << "Unable to Transmit Netlink Message: " << strerror(errno);
    return ret;
  }

  return (ret = true);
}


void dumpData(unsigned char *data,  unsigned int len)
{
  unsigned int uIndx;

  if (data)
  {
    for (uIndx = 0; uIndx < len; ++uIndx)
    {
      if (uIndx % 32 == 0)
      {
        printf("\n%4d:", uIndx);
      }

      if (uIndx % 4 == 0)
      {
        printf(" ");
      }

      //printf("%02x", data[uIndx]);
      printf("%02x", *(data + uIndx));
    }
  }

  printf("\n Length of Bytes: %d\n", len);
  printf("\n");
}


void *recvMsgFromKernel(void *args)
{
  unsigned char ucBuffer[1024];
  struct sockaddr_nl client_addr;
  socklen_t size = sizeof(struct sockaddr_nl);
  int length = 0;

  while (true)
  {
    memset(ucBuffer, 0, 1024);
    // Recieve Data from Kernel
    length = recvfrom(netlinkSockFD, ucBuffer, 1024 , MSG_NOSIGNAL, (
                        struct sockaddr *)&client_addr, &size);

    if (length > 0)
    {
      struct nlmsghdr *nlh = (struct nlmsghdr *) ucBuffer;

      if (0 != nlh->nlmsg_pid)
      {
        printf("\nReceived Message from Unknown Source\n");
        continue;
      }
      else
      {
        unsigned char ucBuf[1024];
        unsigned int unActualLen = length - sizeof(struct nlmsghdr);
        memmove(&ucBuf, NLMSG_DATA(nlh), length - sizeof(struct
                nlmsghdr));
        /* dump Recieved Data */
        dumpData(ucBuf, unActualLen);
      }
    }
  }
}


int main()
{
  bool ret;
  int ch, len, errno;
  /* Intialise Socket Paramters */
  netlinkSockFD = -1;
  PID = getpid();
  SeqNo = 0;
  memset(&DestAddr, 0, sizeof(struct sockaddr_nl));
  memset(&SrcAddr, 0, sizeof(struct sockaddr_nl));

  //create receive thread
  pthread_t threadId;

  if (pthread_create(&threadId, NULL, recvMsgFromKernel, NULL))
  {
    perror("Error in creating receiver thread");
    return -1;
  }

  /* Create NetLink Socket */
  netlinkSockFD = socket(PF_NETLINK, SOCK_RAW, NETLINK_MUNISOCK);

  if (netlinkSockFD == -1)
  {
    cout << "Unable to open socket - " << strerror(errno);
    return 0;
  }
  else
  {
    cout << "Netlink Socket created Successfully\n";
  }

  memset(&DestAddr, 0, sizeof(struct sockaddr_nl));
  DestAddr.nl_family = AF_NETLINK;
  DestAddr.nl_pid = 0; /* For Linux Kernel */
  DestAddr.nl_groups = 0; /* unicast */

  do
  {
    cout << endl;
    cout << " Enter your choice:\t" << endl;
    cout << " 1. Send Data to Kernel" << endl;
    cout << " 2. exit" << endl;
    cin >> ch;
    cout << endl;

    switch (ch)
    {
      case 1:
      {
        cout << "Enter the Length of the Payload " << endl;
        cin >> len;
        ret = sendDataToKernel(len);

        if (ret)
        {
          cout << "\n Netlink Socket Sent successfully \n";
        }
        else
        {
          cout << "\n Netlink Socket Failed in Sending Message \n";
        }

        break;
      }

      default:
        cout << "Invalid Choice\n";
        break;
    }
  }
  while (ch != 2);
}

