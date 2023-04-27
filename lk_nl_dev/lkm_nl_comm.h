
#ifndef LK_NL_COMM_H
#define LK_NL_COMM_H


#include <linux/skbuff.h>

/**
 * struct lk_nl_comm_proto
 *
 * @recv: the receiver callback pointing to a function in the FM Radio driver
 *  called by the lk_nl_comm driver upon receiving relevant data.
 *
 * @reg_complete_cb: callback handler pointing to a function in the FM Radio 
 *  driver handler called by lk_nl_comm when the pending registrations are
 *  complete. The registrations are marked pending, in situations when fw
 *  download is in progress.
 *
 * @write: pointer to a function in lk_nl_comm provided to FMR driver from
 *   lk_nl_comm, to be made use when FMR driver has data to send to userspace.
 *
 * @priv_data: privdate data holder for the FMR driver, sent from the FMR driver
 *   during registration, and sent back on reg_complete_cb and recv.
 *
 * @chnl_id: channel id the protocol driver is interested in, the channel
 *  id is nothing but the 1st byte of the packet in UART frame.
 * 
 * @max_frame_size: size of the largest frame the FM Radio driver can receive.
 *
 * @hdr_len: length of the header structure of the FM Radio driver.
 *
 * @offset_len_in_hdr: this provides the offset of the length field in the
 *  header structure of the protocol header, to assist lk_nl_comm to know
 *  how much to receive, if the data is split across UART frames.
 *
 * @len_size: whether the length field inside the header is 2 bytes
 *  or 1 byte.
 *
 * @reserve: the number of bytes lk_nl_comm needs to reserve in the skb being
 *  prepared for the protocol driver.
 */
struct lk_nl_comm_proto
{
  long (*recv)(void *, struct sk_buff * skb);
  void (*reg_complete_cb)(void *, char data);
  long (*write)(struct sk_buff *skb);

  void *priv_data;

  unsigned char  chnl_id;
  unsigned short max_frame_size;
  unsigned char  hdr_len;
  unsigned char  offset_len_in_hdr;
  unsigned char  len_size;
  unsigned char  reserve;
};

extern long lk_nl_comm_register(struct lk_nl_comm_proto *);
extern long lk_nl_comm_unregister(struct lk_nl_comm_proto *);

#endif
