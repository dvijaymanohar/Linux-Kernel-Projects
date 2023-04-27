
#define KNETLINK_UNIT 17
DEFINE_MUTEX(mut);

static struct sock *knetlink_sk = NULL;
char data_string[] = "Hello Userspace! This is msg from Kernel";

int knetlink_process(struct sk_buff *skb, struct nlmsghdr *nlh)
{
  u8 *payload = NULL;
  int   payload_size;
  int   length;
  int   seq;
  pid_t pid;
  pid = nlh->nlmsg_pid;
  length = nlh->nlmsg_len;
  seq = nlh->nlmsg_seq;
  printk("\nknetlink_process: nlmsg len %d type %d pid %d seq %d\n",
         length, nlh->nlmsg_type, pid, seq);
  payload_size = nlh->nlmsg_len - NLMSG_LENGTH(0);
  
  if (payload_size > 0)
  {
    payload = NLMSG_DATA(nlh);
    printk("\nknetlink_process: Payload is %s ", payload);
  }
  
  struct sk_buff *rskb = alloc_skb(nlh->nlmsg_len, GFP_KERNEL);
  
  if (rskb)
  {
    printk("\nknetlink_process: replies with the same socket_buffer\n");
    *rskb = *skb;
  }
  else
  {
    skb_put(rskb, length);
    kfree_skb(skb);
  }
  
  nlh = (struct nlmsghdr *) rskb->data;
  nlh->nlmsg_len = length;
  nlh->nlmsg_pid = pid;
  nlh->nlmsg_flags = 0;
  nlh->nlmsg_type = 2;
  nlh->nlmsg_seq   = seq++;
  payload = NLMSG_DATA(nlh);
  printk("\nknetlink_process: reply nlmsg len %d type %d pid %d\n",
         nlh->nlmsg_len, nlh->nlmsg_type, nlh->nlmsg_pid);
  //skb_put(skb, NLMSG_SPACE(MAX_PAYLOAD))
  strlcpy(payload, data_string, nlh->nlmsg_len);
  *(payload + strlen(data_string)) = '\0';
  NETLINK_CB(rskb).pid = 0; //from kernel
  netlink_unicast(knetlink_sk, rskb, pid, MSG_DONTWAIT);
  return 0;
}

void knetlink_input(struct sk_buff *skb)
{
  mutex_lock(&mut);
  printk("\nFunction %s() called", __FUNCTION__);
  netlink_rcv_skb(skb, &knetlink_process);
  mutex_unlock(&mut);
}

int knetlink_init(void)
{
  if (knetlink_sk != NULL)
  {
    printk("knetlink_init: sock already present\n");
    return 1;
  }
  
  knetlink_sk = netlink_kernel_create(&init_net, KNETLINK_UNIT, 0, knetlink_input, NULL, THIS_MODULE);
  
  if (knetlink_sk == NULL)
  {
    printk("knetlink_init: sock fail\n");
    return 1;
  }
  
  printk("knetlink_init: sock %p\n", (void *)knetlink_sk);
  return 0;
}

void knetlink_exit(void)
{
  if (knetlink_sk != NULL)
  {
    printk("knetlink_exit: release sock %p\n", (void *)knetlink_sk);
    sock_release(knetlink_sk->sk_socket);
  }
  else
  {
    printk("knetlink_exit: warning sock is NULL\n");
  }
}

module_init(knetlink_init);
module_exit(knetlink_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VMD");

