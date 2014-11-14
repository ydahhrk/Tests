#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include "ipv6_hdr_iterator.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");
MODULE_DESCRIPTION("Packet logger");
MODULE_ALIAS("logger");


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
#define HOOK_ARG_TYPE const struct nf_hook_ops *
#else
#define HOOK_ARG_TYPE unsigned int
#endif



static inline bool is_df_set(const struct iphdr *hdr)
{
	return (be16_to_cpu(hdr->frag_off) & IP_DF) >> 14;
}

static inline bool is_mf6_set(const struct frag_hdr *hdr)
{
	return (be16_to_cpu(hdr->frag_off) & IP6_MF);
}

static inline bool is_mf4_set(const struct iphdr *hdr)
{
	return (be16_to_cpu(hdr->frag_off) & IP_MF) >> 13;
}

static inline __u16 get_fo6(const struct frag_hdr *hdr)
{
	return be16_to_cpu(hdr->frag_off) & 0xFFF8;
}

static inline __u16 get_fo4(const struct iphdr *hdr)
{
	return (be16_to_cpu(hdr->frag_off) & IP_OFFSET) << 3;
}

static void print_payload(struct sk_buff *skb)
{
	unsigned char *x;
	unsigned int y;

	pr_debug("Bytes:\n");
	y = 0;
	for (x = skb->data; x < skb->tail; x++) {
		printk("%u	", *x);
		y++;
		if (y % 10 == 0)
			printk("\n");
	}

	pr_debug("\n");
}

static void print_pkt4(char *prefix, struct sk_buff *skb)
{
	struct iphdr *hdr = ip_hdr(skb);
	struct in_addr addr4;

	pr_info("-----------------\n");

	/* skb pointers */
	pr_info("%s%pI4 %pI4 "
			"len:%u head:%p data:%p tail:%p end:%p "
			"nh:%p th:%p "
			"prev:%p next:%p"
			"DF:%u MF:%u FO:%u "
			"\n",
			prefix, &hdr->saddr, &hdr->daddr,
			skb->len, skb->head, skb->data, skb->tail, skb->end,
			skb_network_header(skb), skb_transport_header(skb),
			skb->prev, skb->next,
			is_df_set(hdr), is_mf4_set(hdr), get_fo4(hdr)
	);
	pr_info("frag_list:%p nr_frags:%u", skb_shinfo(skb)->frag_list,
			skb_shinfo(skb)->nr_frags);

	/* IPv4 header */
	hdr = ip_hdr(skb);
	pr_debug("IPv4 header:\n");
	pr_debug("\t	version: %u\n", hdr->version);
	pr_debug("\t	header length: %u\n", hdr->ihl);
	pr_debug("\t	type of service: %u\n", hdr->tos);
	pr_debug("\t	total length: %u\n", be16_to_cpu(hdr->tot_len));
	pr_debug("\t	identification: %u\n", be16_to_cpu(hdr->id));
	pr_debug("\t	don't fragment: %u\n", is_df_set(hdr));
	pr_debug("\t	more fragments: %u\n", is_mf4_set(hdr));
	pr_debug("\t	fragment offset: %u\n", get_fo4(hdr));
	pr_debug("\t	time to live: %u\n", hdr->ttl);
	pr_debug("\t	protocol: %u\n", hdr->protocol);
	pr_debug("\t	checksum: %x\n", ntohs((__force __be16) hdr->check));
	addr4.s_addr = hdr->saddr;
	pr_debug("		source address: %pI4\n", &addr4);
	addr4.s_addr = hdr->daddr;
	pr_debug("		destination address: %pI4\n", &addr4);

	/* Payload */
	print_payload(skb);

	/* Fragments */
	if (skb_has_frag_list(skb)) {
		struct sk_buff *frag;
		skb_walk_frags(skb, frag) {
			print_pkt4("(sub) ", frag);
		}
	}

	pr_info("-----------------\n");
}

static unsigned int hook_ipv4(HOOK_ARG_TYPE hook, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	/*
	int error;

	error = skb_linearize(skb);
	if (error) {
		pr_debug("could not linearize.");
		return NF_ACCEPT;
	}
	*/

	print_pkt4("", skb);

	return NF_ACCEPT;
}

static void print_pkt6(char *prefix, struct sk_buff *skb)
{
	struct ipv6hdr *hdr6 = ipv6_hdr(skb);
	struct frag_hdr *fhdr = get_extension_header(hdr6, NEXTHDR_FRAGMENT);

	pr_info("-----------------\n");

	/* skb pointers */
	pr_info("%s%pI6c %pI6c "
			"len:%u head:%p data:%p tail:%p end:%p "
			"nh:%p th:%p "
			"\n",
			prefix, &hdr6->saddr, &hdr6->daddr,
			skb->len, skb->head, skb->data, skb->tail, skb->end,
			skb_network_header(skb), skb_transport_header(skb));
	pr_info("frag_list:%p nr_frags:%u", skb_shinfo(skb)->frag_list,
			skb_shinfo(skb)->nr_frags);

	/* IPv6 header */
	hdr6 = ipv6_hdr(skb);
	pr_debug("IPv6 header:\n");
	pr_debug("\t	version: %u\n", hdr6->version);
	pr_debug("\t	traffic class: %u\n", (hdr6->priority << 4)
			| (hdr6->flow_lbl[0] >> 4));
	pr_debug("\t	flow label: %u\n", ((hdr6->flow_lbl[0] & 0xf) << 16)
			| (hdr6->flow_lbl[1] << 8)
			| hdr6->flow_lbl[0]);
	pr_debug("\t	payload length: %u\n", be16_to_cpu(hdr6->payload_len));
	pr_debug("\t	next header: %u\n", hdr6->nexthdr);
	pr_debug("\t	hop limit: %u\n", hdr6->hop_limit);
	pr_debug("\t	source address: %pI6c\n", &hdr6->saddr);
	pr_debug("\t	destination address: %pI6c\n", &hdr6->daddr);

	/* Fragment header */
	if (fhdr) {
		pr_debug("Fragment header:\n");
		pr_debug("\t	next header: %u\n", fhdr->nexthdr);
		pr_debug("\t	reserved: %u\n", fhdr->reserved);
		pr_debug("\t	fragment offset: %u\n", get_fo6(fhdr));
		pr_debug("\t	more fragments: %u\n", is_mf6_set(fhdr));
		pr_debug("\t	identification: %u\n",
				be32_to_cpu(fhdr->identification));
	}

	/* Payload */
	print_payload(skb);

	/* Fragments */
	if (skb_has_frag_list(skb)) {
		struct sk_buff *frag;
		/*
		pr_info("    head:%u page:%u nr_frags:%u\n",
				skb_headlen(skb), skb_pagelen(skb),
				skb_shinfo(skb)->nr_frags);
		 */
		skb_walk_frags(skb, frag) {
			print_pkt6("(sub) ", frag);
		}
	}

	pr_info("-----------------\n");
}

static unsigned int hook_ipv6(HOOK_ARG_TYPE hook, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	/*
	int error;

	error = skb_linearize(skb);
	if (error) {
		pr_debug("could not linearize.");
		return NF_ACCEPT;
	}
	*/

	print_pkt6("", skb);

	return NF_ACCEPT;
}

static struct nf_hook_ops nfho[] = {
	{
		.hook = hook_ipv6,
		.hooknum = NF_INET_POST_ROUTING,
		.pf = PF_INET6,
		.priority = NF_IP6_PRI_LAST,
	},
	{
		.hook = hook_ipv4,
		.hooknum = NF_INET_POST_ROUTING,
		.pf = PF_INET,
		.priority = NF_IP_PRI_LAST,
	}
};

static int __init loggermod_init(void)
{
	int error;

	error = nf_register_hooks(nfho, ARRAY_SIZE(nfho));
	if (error)
		return error;

	pr_info("Module inserted.\n");
	return error;
}

static void __exit loggermod_exit(void)
{
	nf_unregister_hooks(nfho, ARRAY_SIZE(nfho));
	pr_info("Module removed.\n");
}

module_init(loggermod_init);
module_exit(loggermod_exit);
