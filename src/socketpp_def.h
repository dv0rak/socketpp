#ifndef __SOCKETPP_DEF_H
#define __SOCKETPP_DEF_H 1

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

namespace socketpp {

typedef unsigned char _u8;
typedef unsigned short _u16;
typedef unsigned long _u32;
typedef _u16 port_t;

const int inaddr_any 	  = INADDR_ANY;
const int inaddr_broadcast= INADDR_BROADCAST;
const int inaddr_loopback = INADDR_LOOPBACK;
const int inaddr_none	  = INADDR_NONE;

enum type {
    sock_stream	= SOCK_STREAM,
    sock_dgram	= SOCK_DGRAM,
    sock_raw	= SOCK_RAW,
    sock_rdm	= SOCK_RDM,
    sock_seqpacket  = SOCK_SEQPACKET
};
enum protocol {
    ipproto_ip	= IPPROTO_IP,
    ipproto_tcp	= IPPROTO_TCP,
    ipproto_udp	= IPPROTO_UDP,
    ipproto_raw	= IPPROTO_RAW,
    ipproto_icmp= IPPROTO_ICMP
};
enum option {
    ip_hdrincl	= IP_HDRINCL,
    so_reuseaddr= SO_REUSEADDR,
    so_broadcast= SO_BROADCAST,
    so_sndtimeo	= SO_SNDTIMEO,
    so_rcvtimeo	= SO_RCVTIMEO
};	
enum level {
    sol_ip    = SOL_IP,
    sol_socket= SOL_SOCKET 
};

struct icmphdr {
    _u8  type;
    _u8  code;
    _u16 check;
    _u16 id;
    _u16 sequence;
};

const int IPSIZE         = sizeof(struct iphdr);
const int TCPSIZE        = sizeof(struct tcphdr);
const int UDPSIZE        = sizeof(struct udphdr);
const int ICMPSIZE       = sizeof(struct icmphdr);
const int MAX_PACKET_SIZE= 65535;

};

#endif
