#include <linux/kernel.h>
#include <linux/module.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>

#include "nlexample.h"

#define NETLINK_USER 31
static struct netlink_kernel_cfg cfg = {0};

static struct sock *nlsk;
static int myvar;

static int
nlex_unicast(int pid)
{
  struct sk_buff *skb;
  struct nlmsghdr *nlh;
  skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);

  if (skb == NULL)
  {
    return -ENOMEM;
  }

  nlh = nlmsg_put(skb, pid, 0, NLEX_MSG_UPD, 0, 0);

  if (nlh == NULL)
  {
    goto nlmsg_failure;
  }

  NLA_PUT_U32(skb, NLE_MYVAR, myvar);
  nlmsg_end(skb, nlh);

  return nlmsg_unicast(nlsk, skb, pid);

nlmsg_failure:
nla_put_failure:
  nlmsg_cancel(skb, nlh);
  kfree_skb(skb);

  return -1;
}

static int
nlex_notify(int rep, int pid)
{
  struct sk_buff *skb;
  struct nlmsghdr *nlh;
  skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);

  if (skb == NULL)
  {
    return -ENOMEM;
  }

  nlh = nlmsg_put(skb, pid, rep, NLEX_MSG_UPD, 0, 0);

  if (nlh == NULL)
  {
    goto nlmsg_failure;
  }

  NLA_PUT_U32(skb, NLE_MYVAR, myvar);
  nlmsg_end(skb, nlh);
  return nlmsg_notify(nlsk, skb, pid,
                      NLEX_GRP_MYVAR,
                      rep, GFP_KERNEL);
nlmsg_failure:
nla_put_failure:
  nlmsg_cancel(skb, nlh);
  kfree_skb(skb);
  return -1;
}

static int
nl_step2(struct nlattr *cda[],
         struct nlmsghdr *nlh)
{
  int echo = nlh->nlmsg_flags & NLM_F_ECHO;
  int pid = nlh->nlmsg_pid;

  switch (nlh->nlmsg_type)
  {
    case NLEX_MSG_UPD:
      if (!cda[NLE_MYVAR])
      {
        return -EINVAL;
      }

      myvar = nla_get_u32(cda[NLE_MYVAR]);
      nlex_notify(echo, pid);
      break;

    case NLEX_MSG_GET:
      nlex_unicast(pid);
      break;
  };

  return 0;
}

static const struct nla_policy nle_info_policy[NLE_MAX + 1] =
{
  [NLE_MYVAR] = { .type = NLA_U32 },
};

static int nl_step(struct sk_buff *skb, struct nlmsghdr *nlh)
{
  int err;
  struct nlattr *cda[NLE_MAX + 1];
  struct nlattr *attr = NLMSG_DATA(nlh);
  int attrlen = nlh->nlmsg_len - NLMSG_SPACE(0);

  if (security_netlink_recv(skb, CAP_NET_ADMIN))
  {
    return -EPERM;
  }

  if (nlh->nlmsg_len < NLMSG_SPACE(0))
  {
    return -EINVAL;
  }

  err = nla_parse(cda, NLE_MAX,
                  attr, attrlen, nle_info_policy);

  if (err < 0)
  {
    return err;
  }

  return nl_step2(cda, nlh);
}

static void nl_callback(struct sk_buff *skb)
{
  netlink_rcv_skb(skb, &nl_step);
}


static int __init nlexample_init(void)
{
#if 0
  nlsk = netlink_kernel_create(&init_net,
                               NETLINK_EXAMPLE,
                               NLEX_GRP_MYVAR,
                               nl_callback,
                               NULL,
                               THIS_MODULE);
#endif

  /* Tell Netlink where our callback function is */
  cfg.input = nl_callback;

  /* Create a Netlink socket */
  nlsk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

  if (nlsk == NULL)
  {
    printk("Can't create netlink\n");
    return -ENOMEM;
  }

  return 0;
}

void __exit nlexample_exit(void)
{
  netlink_kernel_release(nlsk);
}

module_init(nlexample_init);
module_exit(nlexample_exit);

MODULE_AUTHOR("Pablo Neira Ayuso <pablo@netfilter.org>");
MODULE_LICENSE("GPL");
