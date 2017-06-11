#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER_RECIEVE 31
#define NETLINK_USER_SEND 17

MODULE_LICENSE("GPL");

struct sock *skrecieve = NULL;
struct sock *sksend = NULL;

/* Message Place Holder */
static char msg[1024];

static void recv_msg(struct sk_buff *skb)
{
	/* Print out function name into log */
	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

	/* Netlink Message Header */
	struct nlmsghdr *nlh;

	/* Get Data from Buffer and put it into header */
	nlh=(struct nlmsghdr*)skb->data;

	/* Copy message data to message place holder */
	strncpy(msg,(char*)nlmsg_data(nlh),1024);

	/* Print out the message to log */
	printk(KERN_INFO "Got the msg!!!! It is: %s\n", (char*)nlmsg_data(nlh));
}

static void send_msg(struct sk_buff *skb)
{
	/* Print out function name into log */
	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

	/* Netlink Message Header */
	struct nlmsghdr *nlh;

	/* Process ID */
	int pid;

	/* Buffer */
	struct sk_buff *skb_out;

	/* Message Size */
	int msg_size;

	/* Status */
	int res;
	
	/* Set Message Size */
	msg_size=strlen(msg);

	/* Get Data from Buffer and put it into header */
	nlh=(struct nlmsghdr*)skb->data;
	printk(KERN_INFO "Got the request!\n");

	/* Set PID of Sending Process */
	pid = nlh->nlmsg_pid; 

	/* Set Buffer Size of Send */
	skb_out = nlmsg_new(msg_size,0);

	if(!skb_out)
{	
		printk(KERN_ERR "NO MORE SPACE BRO!\n");
		return;
	} 

	/* Copy Settings from Buffer */
	nlh=nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);

	/* Set to broadcast as UNICAST */
	NETLINK_CB(skb_out).dst_group = 0;

	/* Copy message into Netlink Message Buffer */ 
	strncpy(nlmsg_data(nlh),msg,msg_size);

	/* Send the message */
	res=nlmsg_unicast(sksend,skb_out,pid);

	if(res<0)
		printk(KERN_INFO "Where is the user!?!?!?!?!?\n");

	/* Erase Message Place Holder Contents */
	*msg = NULL;
}

static int __init netinit(void)
{
	/* Print out function name into log */ 
	printk("Entering: %s\n",__FUNCTION__);

	/* Start the NETLINK functions */
	skrecieve=netlink_kernel_create(&init_net, NETLINK_USER_RECIEVE, 0, recv_msg, NULL, THIS_MODULE);
	sksend=netlink_kernel_create(&init_net, NETLINK_USER_SEND, 0, send_msg, NULL, THIS_MODULE);

	if(!skrecieve)
	{
		printk(KERN_ALERT "NO!!!!! CAN'T CREATE THE RECIEVE SOCKET!!!!! FAIL!!!!\n");
		return -10;
	}
	if(!sksend)
	{
		printk(KERN_ALERT "NO!!!!! CAN'T CREATE THE SEND SOCKET!!!!! FAIL!!!!\n");
		return -10;
	}
	return 0;
}

static void __exit netexit(void){
	printk(KERN_INFO "BUH BUY BRO!!!\n");
	netlink_kernel_release(skrecieve);
	netlink_kernel_release(sksend);
}

module_init(netinit);
module_exit(netexit);


