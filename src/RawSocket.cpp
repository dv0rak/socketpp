#include "RawSocket.h"
#include <algorithm>

namespace socketpp {

void RawSocket::build_data_payload(const char *buf, size_t len)
{
    data_payload.assign(buf,len);
}

void RawSocket::build_data_payload(const std::string &buf)
{
    data_payload = buf;
}

size_t RawSocket::send_packet(in_addr_t addr)
{
    std::string packet;
    _build_packet(packet);
    return sendto(packet,addr);
}
    
size_t RawSocket::send_packet(const std::string& addr)
{
    std::string packet;
    _build_packet(packet);
    return sendto(packet,addr);
}

size_t RawSocket::send_packet()
{
    std::string packet;
    _build_packet(packet);
    return send(packet);
}

size_t RawSocket::read_packet(size_t len, std::string &addr)
{   
    std::string packet;
    int n=recvfrom(packet,len,addr);
    _set_fields(packet);
    return n;
} 

size_t RawSocket::read_packet(size_t len, in_addr_t &addr)
{
    std::string packet;
    int n=recvfrom(packet,len,addr);
    _set_fields(packet);
    return n;
}

size_t RawSocket::read_packet(size_t len)
{
    std::string packet;
    int n=recv(packet,len);
    _set_fields(packet);
    return n;
}

size_t RawSocket::get_data_payload(char *buf,size_t len)
{
    int n = std::min(rcvd_data_payload.size(), len);
    memcpy(buf, rcvd_data_payload.c_str(), n);
    return n;
}

size_t RawSocket::get_data_payload(std::string &buf)
{
    buf = rcvd_data_payload;
    return buf.size();
}

_u16 RawSocket::checksum(const void *buf, size_t len)
{
    register _u32 sum=0;

    if(len&1)
        sum += ((char *)buf)[len-1];

    len >>= 1;
    _u16 *ubuf = (_u16 *)buf; 
 
    while(len-- > 0) 
        sum += *ubuf++;
    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);
    return ~sum;
}

_u16 RawSocket::checksum(const std::string &buf)
{
    return checksum(buf.c_str(),buf.size());
}

_u16 RawSocket::pseudo_checksum(const void *buf, size_t len, _u32 saddr, _u32 daddr, _u16 proto)
{
    register _u32 sum=0;
    _u16 *src=(_u16 *)&saddr, *dst=(_u16 *)&daddr;
    saddr=::htonl(saddr); daddr=::htonl(daddr);
    
    sum += src[0];
    sum += src[1];
    sum += dst[0];
    sum += dst[1];
    sum += htons(proto);
    sum += htons(len);

    if(len&1)
        sum += ((char *)buf)[len-1];

    len >>= 1;
    _u16 *ubuf = (_u16 *)buf;

    while(len-- > 0) 
        sum += *ubuf++;
    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);
    return ~sum;
}

_u16 TCP_RawSocket::tcp_checksum(const void *buf, size_t len, _u32 saddr, _u32 daddr)
{
    return pseudo_checksum(buf,len,saddr,daddr,IPPROTO_TCP);
}

_u16 UDP_RawSocket::udp_checksum(const void *buf, size_t len, _u32 saddr, _u32 daddr)
{
    return pseudo_checksum(buf,len,saddr,daddr,IPPROTO_UDP);
}

_u16 TCP_RawSocket::tcp_checksum(const std::string &buf, _u32 saddr, _u32 daddr)
{
    return pseudo_checksum(buf.c_str(),buf.size(),saddr,daddr,IPPROTO_TCP);
}

_u16 UDP_RawSocket::udp_checksum(const std::string &buf, _u32 saddr, _u32 daddr)
{
    return pseudo_checksum(buf.c_str(),buf.size(),saddr,daddr,IPPROTO_UDP);
}

void IP_RawSocket::build_IP_header(const iphdr &ip)
{
    IP_h.ihl	   = ip.ihl;
    IP_h.version   = ip.version;
    IP_h.tos	   = ip.tos;
    IP_h.tot_len   = ::ntohs(ip.tot_len);
    IP_h.id	   = ::ntohs(ip.id);
    IP_h.frag_off  = ::ntohs(ip.frag_off);
    IP_h.ttl	   = ip.ttl;
    IP_h.protocol  = ip.protocol;
    IP_h.check	   = ::ntohs(ip.check);  	 
    IP_h.saddr	   = ::ntohl(ip.saddr);
    IP_h.daddr	   = ::ntohl(ip.daddr);
}

void IP_RawSocket::build_IP_header(_u32 saddr, _u32 daddr, _u8 ttl, _u8 version, _u8 protocol, _u16 id, 
                                   _u8 tos, _u16 frag_off, _u8 ihl, _u16 tot_len, _u16 check)
{
    IP_h.ihl	   = ihl;
    IP_h.version   = version;
    IP_h.tos	   = tos;
    IP_h.tot_len   = ::ntohs(tot_len);
    IP_h.id	   = ::ntohs(id);
    IP_h.frag_off  = ::ntohs(frag_off);
    IP_h.ttl	   = ttl;
    IP_h.protocol  = protocol;
    IP_h.check	   = ::ntohs(check);		  
    IP_h.saddr	   = ::ntohl(saddr);
    IP_h.daddr	   = ::ntohl(daddr);
}

void IP_RawSocket::set_IP_options(const char *opt, size_t len)
{
    IP_opt.assign(opt,len);
}

void IP_RawSocket::set_IP_options(const std::string &opt)
{
    IP_opt = opt;
}

void IP_RawSocket::adjust_IP_csum()
{
    IP_h.check = 0;
    std::string packet;
    _build_packet(packet);
    IP_h.check = checksum(packet);		 	
}

void IP_RawSocket::adjust_IP_ihl()
{
    IP_h.ihl = (IPSIZE + IP_opt.size())/4;
}

void IP_RawSocket::adjust_IP_tot_len()
{
    std::string packet;
    _build_packet(packet);
    IP_h.tot_len = ::htons(packet.size());
}

void IP_RawSocket::adjust_IP_all()
{
    adjust_IP_proto();
    adjust_IP_ihl();
    adjust_IP_tot_len();
    adjust_IP_csum();
}

void IP_RawSocket::get_IP_header(iphdr &ip)
{
    ip.ihl	= rcvd_IP_h.ihl;
    ip.version  = rcvd_IP_h.version;
    ip.tos	= rcvd_IP_h.tos;
    ip.tot_len  = ::ntohs(rcvd_IP_h.tot_len);
    ip.id	= ::ntohs(rcvd_IP_h.id);
    ip.frag_off = ::ntohs(rcvd_IP_h.frag_off);
    ip.ttl	= rcvd_IP_h.ttl;
    ip.protocol = rcvd_IP_h.protocol;
    ip.check	= ::ntohs(rcvd_IP_h.check); 	 	 
    ip.saddr	= ::ntohl(rcvd_IP_h.saddr);
    ip.daddr	= ::ntohl(rcvd_IP_h.daddr);
}

size_t IP_RawSocket::get_IP_options(char *buf,size_t len)
{
    int n = std::min(rcvd_IP_opt.size(), len);
    memcpy(buf, rcvd_IP_opt.c_str(), n);
    return n;
}
    
size_t IP_RawSocket::get_IP_options(std::string &buf)
{
    buf = rcvd_IP_opt;
    return buf.size();
}

void IP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&IP_h, IPSIZE);
    packet += IP_opt + data_payload;
}
    
void IP_RawSocket::_set_fields(const std::string &packet)
{
    rcvd_IP_h = *(iphdr *)packet.c_str();
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4);
}

void ICMP_RawSocket::build_ICMP_header(const icmphdr &icmp)
{
    ICMP_h.type	    = icmp.type;
    ICMP_h.code	    = icmp.code;
    ICMP_h.check    = icmp.check; 				  
    ICMP_h.id	    = ::htons(icmp.id);
    ICMP_h.sequence = ::htons(icmp.sequence);
}

void ICMP_RawSocket::build_ICMP_header(_u8 type, _u8 code, _u16 id, _u16 sequence, _u16 check)
{
    ICMP_h.type	    = type;
    ICMP_h.code	    = code;
    ICMP_h.check    = check; 				  
    ICMP_h.id	    = ::htons(id);
    ICMP_h.sequence = ::htons(sequence);
}

void ICMP_RawSocket::get_ICMP_header(icmphdr &icmp)
{
    icmp.type	    = rcvd_ICMP_h.type;
    icmp.code	    = rcvd_ICMP_h.code;
    icmp.check      = rcvd_ICMP_h.check;			
    icmp.id	    = ::htons(rcvd_ICMP_h.id);
    icmp.sequence   = ::htons(rcvd_ICMP_h.sequence);
}

void ICMP_RawSocket::adjust_ICMP_csum()
{
    ICMP_h.check = 0;
    std::string packet ((char *)&ICMP_h, ICMPSIZE);
                packet += data_payload;
    ICMP_h.check = checksum(packet); 			     
}

void ICMP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&ICMP_h, ICMPSIZE);
    packet += data_payload;
}
    
void ICMP_RawSocket::_set_fields(const std::string &packet)
{
    iphdr *ip = (iphdr *)packet.c_str();
    rcvd_ICMP_h = *(icmphdr *)(packet.c_str() +ip->ihl*4);
    rcvd_data_payload = packet.substr(ip->ihl*4 +ICMPSIZE);
}

ICMP_IP_RawSocket::ICMP_IP_RawSocket() : RawSocket(ipproto_icmp)
{
    setsockopt(sol_ip, ip_hdrincl, 1);
}

void ICMP_IP_RawSocket::adjust_ICMP_IP_all()
{
    adjust_ICMP_csum();
    adjust_IP_all();
}

void ICMP_IP_RawSocket::adjust_IP_proto()
{
    IP_h.protocol = IPPROTO_ICMP;
}

void ICMP_IP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&IP_h, IPSIZE);
    packet.append(IP_opt);
    packet.append((char *)&ICMP_h, ICMPSIZE);
    packet.append(data_payload);
}

void ICMP_IP_RawSocket::_set_fields(const std::string &packet)
{
    rcvd_IP_h = *(iphdr *)packet.c_str();
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    rcvd_ICMP_h = *(icmphdr *)(packet.c_str() + rcvd_IP_h.ihl*4);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4 + ICMPSIZE);
}

void UDP_RawSocket::build_UDP_header(_u16 source, _u16 dest, _u16 len, _u16 check)
{
    UDP_h.source    = ::htons(source);
    UDP_h.dest	    = ::htons(dest);
    UDP_h.len	    = ::htons(len);
    UDP_h.check	    = check;
}

void UDP_RawSocket::build_UDP_header(const udphdr &udp)
{
    UDP_h.source    = ::htons(udp.source);
    UDP_h.dest	    = ::htons(udp.dest);
    UDP_h.len	    = ::htons(udp.len);
    UDP_h.check	    = udp.check;
}

void UDP_RawSocket::adjust_UDP_csum(in_addr_t source, in_addr_t dest)
{
    UDP_h.check = 0;
    std::string packet ((char *)&UDP_h, UDPSIZE);
                packet += data_payload;
    UDP_h.check = udp_checksum(packet,source,dest);
}

void UDP_RawSocket::adjust_UDP_len()
{
    UDP_h.len = ::htons(UDPSIZE + data_payload.size());
}

void UDP_RawSocket::adjust_UDP_all(in_addr_t source, in_addr_t dest)
{
    adjust_UDP_len();
    adjust_UDP_csum(source,dest);
}

void UDP_RawSocket::get_UDP_header(udphdr &udp)
{
    udp.source = ::htons(rcvd_UDP_h.source);
    udp.dest   = ::htons(rcvd_UDP_h.dest);
    udp.len    = ::htons(rcvd_UDP_h.len);
    udp.check  = rcvd_UDP_h.check;
}

void UDP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&UDP_h, UDPSIZE);
    packet += data_payload;
}

void UDP_RawSocket::_set_fields(const std::string &packet)
{
    iphdr *ip = (iphdr *)packet.c_str();
    rcvd_UDP_h = *(udphdr *)(packet.c_str() +ip->ihl*4);
    rcvd_data_payload = packet.substr(ip->ihl*4 +UDPSIZE);
}

UDP_IP_RawSocket::UDP_IP_RawSocket() : RawSocket(ipproto_udp)
{
    setsockopt(sol_ip, ip_hdrincl, 1);
}

void UDP_IP_RawSocket::adjust_UDP_csum()
{
    UDP_RawSocket::adjust_UDP_csum(::htonl(IP_h.saddr),::htonl(IP_h.daddr));
}

void UDP_IP_RawSocket::adjust_UDP_all()
{
    adjust_UDP_len();
    adjust_UDP_csum();
}

void UDP_IP_RawSocket::adjust_UDP_IP_all()
{
    adjust_UDP_all();
    adjust_IP_all();
}

void UDP_IP_RawSocket::adjust_IP_proto()
{
    IP_h.protocol = IPPROTO_UDP;
}

void UDP_IP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&IP_h, IPSIZE);
    packet.append(IP_opt);
    packet.append((char *)&UDP_h, UDPSIZE);
    packet.append(data_payload);
}

void UDP_IP_RawSocket::_set_fields(const std::string &packet)
{
    rcvd_IP_h = *(iphdr *)packet.c_str();
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    rcvd_UDP_h = *(udphdr *)(packet.c_str() + rcvd_IP_h.ihl*4);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4 +UDPSIZE);
}

void TCP_RawSocket::build_TCP_header(_u16 source, _u16 dest, _u32 seq, _u32 ack_seq, _u16 window,
                                     _u8 fin, _u8 syn, _u8 rst, _u8 psh, _u8 ack, _u8 urg,
                                     _u8 ece, _u8 cwr, _u8 doff, _u16 urg_ptr, _u16 check, _u8 res1)
{
    TCP_h.source   = ::htons(source);
    TCP_h.dest	   = ::htons(dest);
    TCP_h.seq      = ::htonl(seq);
    TCP_h.ack_seq  = ::htonl(ack_seq);
    TCP_h.res1     = res1;
    TCP_h.doff     = doff;
    TCP_h.fin      = fin;
    TCP_h.syn      = syn;
    TCP_h.rst      = rst;
    TCP_h.psh      = psh;
    TCP_h.ack      = ack;
    TCP_h.urg      = urg;
    TCP_h.ece      = ece;
    TCP_h.cwr      = cwr;
    TCP_h.window   = ::htons(window);
    TCP_h.check    = check;
    TCP_h.urg_ptr  = ::htons(urg_ptr);
}

void TCP_RawSocket::build_TCP_header(const tcphdr &tcp)
{
    TCP_h.source   = ::htons(tcp.source);
    TCP_h.dest	   = ::htons(tcp.dest);
    TCP_h.seq      = ::htonl(tcp.seq);
    TCP_h.ack_seq  = ::htonl(tcp.ack_seq);
    TCP_h.res1     = tcp.res1;
    TCP_h.doff     = tcp.doff;
    TCP_h.fin      = tcp.fin;
    TCP_h.syn      = tcp.syn;
    TCP_h.rst      = tcp.rst;
    TCP_h.psh      = tcp.psh;
    TCP_h.ack      = tcp.ack;
    TCP_h.urg      = tcp.urg;
    TCP_h.ece      = tcp.ece;
    TCP_h.cwr      = tcp.cwr;
    TCP_h.window   = ::htons(tcp.window);
    TCP_h.check    = tcp.check;
    TCP_h.urg_ptr  = ::htons(tcp.urg_ptr);
}
    
void TCP_RawSocket::set_TCP_options(const char *opt, size_t len)
{
    TCP_opt.assign(opt,len);
}

void TCP_RawSocket::set_TCP_options(const std::string& opt)
{
    TCP_opt = opt;
}

void TCP_RawSocket::adjust_TCP_csum(in_addr_t source, in_addr_t dest)
{
    TCP_h.check = 0;
    std::string packet ((char *)&TCP_h, TCPSIZE);
		packet += TCP_opt;
                packet += data_payload;
    TCP_h.check = tcp_checksum(packet,source,dest);
}

void TCP_RawSocket::adjust_TCP_doff()
{
    TCP_h.doff = (TCPSIZE + TCP_opt.size())/4;
}

void TCP_RawSocket::adjust_TCP_all(in_addr_t source, in_addr_t dest)
{
    adjust_TCP_doff();
    adjust_TCP_csum(source,dest);
}

void TCP_RawSocket::get_TCP_header(tcphdr &tcp)
{
    tcp.source   = ::htons(rcvd_TCP_h.source);
    tcp.dest	 = ::htons(rcvd_TCP_h.dest);
    tcp.seq      = ::htonl(rcvd_TCP_h.seq);
    tcp.ack_seq  = ::htonl(rcvd_TCP_h.ack_seq);
    tcp.res1     = rcvd_TCP_h.res1;
    tcp.doff     = rcvd_TCP_h.doff;
    tcp.fin      = rcvd_TCP_h.fin;
    tcp.syn      = rcvd_TCP_h.syn;
    tcp.rst      = rcvd_TCP_h.rst;
    tcp.psh      = rcvd_TCP_h.psh;
    tcp.ack      = rcvd_TCP_h.ack;
    tcp.urg      = rcvd_TCP_h.urg;
    tcp.ece      = rcvd_TCP_h.ece;
    tcp.cwr      = rcvd_TCP_h.cwr;
    tcp.window   = ::htons(rcvd_TCP_h.window);
    tcp.check    = rcvd_TCP_h.check;
    tcp.urg_ptr  = ::htons(rcvd_TCP_h.urg_ptr);
}

size_t TCP_RawSocket::get_TCP_options(char *opt,size_t len)
{
    int n = std::min(len,rcvd_TCP_opt.size());
    memcpy(opt,rcvd_TCP_opt.c_str(),n);
    return n;
}

size_t TCP_RawSocket::get_TCP_options(std::string &opt)
{
    opt = rcvd_TCP_opt;
    return opt.size();
}

void TCP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&TCP_h, TCPSIZE);
    packet += TCP_opt;
    packet += data_payload;
}

void TCP_RawSocket::_set_fields(const std::string &packet)
{
    iphdr *ip = (iphdr *)packet.c_str();
    rcvd_TCP_h = *(tcphdr *)(packet.c_str() +ip->ihl*4);
    rcvd_TCP_opt = packet.substr(ip->ihl*4 +TCPSIZE, rcvd_TCP_h.doff*4 -TCPSIZE);
    rcvd_data_payload = packet.substr(ip->ihl*4 +rcvd_TCP_h.doff*4);
}

TCP_IP_RawSocket::TCP_IP_RawSocket() : RawSocket(ipproto_tcp)
{
    setsockopt(sol_ip, ip_hdrincl, 1);
}

void TCP_IP_RawSocket::adjust_TCP_csum()
{
    TCP_RawSocket::adjust_TCP_csum(::htonl(IP_h.saddr),::htonl(IP_h.daddr));
}

void TCP_IP_RawSocket::adjust_TCP_all()
{
    adjust_TCP_doff();
    adjust_TCP_csum();
}

void TCP_IP_RawSocket::adjust_IP_proto()
{
    IP_h.protocol = IPPROTO_TCP;
}

void TCP_IP_RawSocket::adjust_TCP_IP_all()
{
    adjust_TCP_all();
    adjust_IP_all();
}

void TCP_IP_RawSocket::_build_packet(std::string &packet)
{
    packet.assign((char *)&IP_h, IPSIZE);
    packet.append(IP_opt);
    packet.append((char *)&TCP_h, TCPSIZE);
    packet.append(TCP_opt);
    packet.append(data_payload);
}

void TCP_IP_RawSocket::_set_fields(const std::string &packet)
{
    rcvd_IP_h = *(iphdr *)packet.c_str();
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    rcvd_TCP_h = *(tcphdr *)(packet.c_str() + rcvd_IP_h.ihl*4);
    rcvd_TCP_opt = packet.substr(rcvd_IP_h.ihl*4 +TCPSIZE, rcvd_TCP_h.doff*4 -TCPSIZE);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4 +rcvd_TCP_h.doff*4);
}

};
