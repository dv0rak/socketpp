#include "RawSocket.h"
#include <algorithm>

namespace socketpp {

void RawSocket::build_data_payload(const char *buf, size_t len)
{
    data_payload.assign(buf, len);
}

void RawSocket::build_data_payload(const std::string &buf)
{
    data_payload = buf;
}

size_t RawSocket::send_packet(in_addr_t addr)
{
    std::string packet;
    _build_packet(packet);
    return sendto(packet, addr);
}
    
size_t RawSocket::send_packet(const std::string& addr)
{
    std::string packet;
    _build_packet(packet);
    return sendto(packet, addr);
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
    int n = recvfrom(packet,len,addr);
    _set_fields(packet);
    return n;
} 

size_t RawSocket::read_packet(size_t len, in_addr_t &addr)
{
    std::string packet;
    int n = recvfrom(packet, len, addr);
    _set_fields(packet);
    return n;
}

size_t RawSocket::read_packet(size_t len)
{
    std::string packet;
    int n = recv(packet,len);
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

void IP_RawSocket::build_IP_header(const iphdr &ip)
{
    IP_h = ip;
}

void IP_RawSocket::build_IP_header(_u32 saddr, _u32 daddr, _u8 ttl, _u8 version, _u8 protocol, _u16 id, 
                                   _u8 tos, _u16 frag_off, _u8 ihl, _u16 tot_len, _u16 check)
{
    IP_h.ihl	   = ihl;
    IP_h.version   = version;
    IP_h.tos	   = tos;
    IP_h.tot_len   = tot_len;
    IP_h.id	   = id;
    IP_h.frag_off  = frag_off;
    IP_h.ttl	   = ttl;
    IP_h.protocol  = protocol;
    IP_h.check	   = check;		  
    IP_h.saddr	   = saddr;
    IP_h.daddr	   = daddr;
}

void IP_RawSocket::set_IP_options(const char *opt, size_t len)
{
    IP_opt.assign(opt, len);
}

void IP_RawSocket::set_IP_options(const std::string &opt)
{
    IP_opt = opt;
}

void IP_RawSocket::adjust_IP_csum()
{
    IP_h.check = 0;
    IP_h.check = ip_checksum(IP_h, data_payload, IP_opt);
}

void TCP_IP_RawSocket::adjust_IP_csum()
{
    IP_h.check = 0;
    IP_h.check = ip_checksum(IP_h, TCP_h, data_payload, IP_opt, TCP_opt);
}

void UDP_IP_RawSocket::adjust_IP_csum()
{
    IP_h.check = 0;
    IP_h.check = ip_checksum(IP_h, UDP_h, data_payload, IP_opt);
}

void ICMP_IP_RawSocket::adjust_IP_csum()
{
    IP_h.check = 0;
    IP_h.check = ip_checksum(IP_h, ICMP_h, data_payload, IP_opt);
}
    
void IP_RawSocket::adjust_IP_ihl()
{
    IP_h.ihl = (IPSIZE + IP_opt.size())/4;
}

void IP_RawSocket::adjust_IP_tot_len()
{
    IP_h.tot_len = IPSIZE + IP_opt.size() + data_payload.size();
}

void TCP_IP_RawSocket::adjust_IP_tot_len()
{
    IP_h.tot_len = IPSIZE + IP_opt.size() + data_payload.size() + TCPSIZE + TCP_opt.size();
}

void UDP_IP_RawSocket::adjust_IP_tot_len()
{
    IP_h.tot_len = IPSIZE + IP_opt.size() + data_payload.size() + UDPSIZE;
}

void ICMP_IP_RawSocket::adjust_IP_tot_len()
{
    IP_h.tot_len = IPSIZE + IP_opt.size() + data_payload.size() + ICMPSIZE;
}

void IP_RawSocket::adjust_IP_all()
{
    adjust_IP_ihl();
    adjust_IP_tot_len();
    adjust_IP_csum();
}

void IP_RawSocket::get_IP_header(iphdr &ip)
{
    ip = rcvd_IP_h;
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

void IP_RawSocket::_build_packet(std::string &packet) const
{
    struct iphdr ip;
    _ip_convert(IP_h, ip);
    packet.assign((char *)&ip, IPSIZE);
    packet += IP_opt + data_payload;
}
    
void IP_RawSocket::_set_fields(const std::string &packet)
{
    _ip_convert(*(iphdr *)packet.c_str(), rcvd_IP_h);
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4);
}

void ICMP_RawSocket::build_ICMP_header(const icmphdr &icmp)
{
    ICMP_h = icmp;
}

void ICMP_RawSocket::build_ICMP_header(_u8 type, _u8 code, _u16 id, _u16 sequence, _u16 check)
{
    ICMP_h.type	    = type;
    ICMP_h.code	    = code;
    ICMP_h.check    = check; 				  
    ICMP_h.id	    = id;
    ICMP_h.sequence = sequence;
}

void ICMP_RawSocket::get_ICMP_header(icmphdr &icmp)
{
    icmp = rcvd_ICMP_h;
}

void ICMP_RawSocket::adjust_ICMP_csum()
{
    ICMP_h.check = 0;
    ICMP_h.check = icmp_checksum(ICMP_h, data_payload); 			     
}

void ICMP_RawSocket::_build_packet(std::string &packet) const
{
    icmphdr icmp;
    _icmp_convert(ICMP_h, icmp);
    packet.assign((char *)&icmp, ICMPSIZE);
    packet += data_payload;
}
    
void ICMP_RawSocket::_set_fields(const std::string &packet)
{
    iphdr *ip = (iphdr *)packet.c_str();
    _icmp_convert(*(icmphdr *)(packet.c_str() +ip->ihl*4), rcvd_ICMP_h);
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

void ICMP_IP_RawSocket::_build_packet(std::string &packet) const
{
    iphdr ip;
    struct icmphdr icmp;
    _ip_convert(IP_h, ip);
    _icmp_convert(ICMP_h, icmp);
    packet.assign((char *)&ip, IPSIZE);
    packet += IP_opt;
    packet.append((char *)&icmp, ICMPSIZE);
    packet += data_payload;
}

void ICMP_IP_RawSocket::_set_fields(const std::string &packet)
{
    _ip_convert(*(iphdr *)packet.c_str(), rcvd_IP_h);
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    _icmp_convert(*(icmphdr *)(packet.c_str() + rcvd_IP_h.ihl*4), rcvd_ICMP_h);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4 + ICMPSIZE);
}

void UDP_RawSocket::build_UDP_header(_u16 source, _u16 dest, _u16 len, _u16 check)
{
    UDP_h.source    = source;
    UDP_h.dest	    = dest;
    UDP_h.len	    = len;
    UDP_h.check	    = check;
}

void UDP_RawSocket::build_UDP_header(const udphdr &udp)
{
    UDP_h = udp;
}

void UDP_RawSocket::adjust_UDP_csum(in_addr_t source, in_addr_t dest)
{
    UDP_h.check = 0;
    UDP_h.check = udp_checksum(UDP_h, source, dest, data_payload);
}

void UDP_RawSocket::adjust_UDP_len()
{
    UDP_h.len = UDPSIZE + data_payload.size();
}

void UDP_RawSocket::adjust_UDP_all(in_addr_t source, in_addr_t dest)
{
    adjust_UDP_len();
    adjust_UDP_csum(source, dest);
}

void UDP_RawSocket::get_UDP_header(udphdr &udp)
{
    udp = rcvd_UDP_h;
}

void UDP_RawSocket::_build_packet(std::string &packet) const
{
    udphdr udp;
    _udp_convert(UDP_h, udp);
    packet.assign((char *)&udp, UDPSIZE);
    packet += data_payload;
}

void UDP_RawSocket::_set_fields(const std::string &packet)
{
    iphdr *ip = (iphdr *)packet.c_str();
    _udp_convert(*(udphdr *)(packet.c_str() +ip->ihl*4), rcvd_UDP_h);
    rcvd_data_payload = packet.substr(ip->ihl*4 +UDPSIZE);
}

UDP_IP_RawSocket::UDP_IP_RawSocket() : RawSocket(ipproto_udp)
{
    setsockopt(sol_ip, ip_hdrincl, 1);
}

void UDP_IP_RawSocket::adjust_UDP_csum()
{
    UDP_RawSocket::adjust_UDP_csum(IP_h.saddr, IP_h.daddr);
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

void UDP_IP_RawSocket::_build_packet(std::string &packet) const
{
    iphdr ip;
    udphdr udp;
    _ip_convert(IP_h, ip);
    _udp_convert(UDP_h, udp);
    packet.assign((char *)&ip, IPSIZE);
    packet += IP_opt;
    packet.append((char *)&udp, UDPSIZE);
    packet += data_payload;
}

void UDP_IP_RawSocket::_set_fields(const std::string &packet)
{
    _ip_convert(*(iphdr *)packet.c_str(), rcvd_IP_h);
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    _udp_convert(*(udphdr *)(packet.c_str() + rcvd_IP_h.ihl*4), rcvd_UDP_h);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4 +UDPSIZE);
}

void TCP_RawSocket::build_TCP_header(_u16 source, _u16 dest, _u32 seq, _u32 ack_seq, _u16 window,
                                     _u8 fin, _u8 syn, _u8 rst, _u8 psh, _u8 ack, _u8 urg,
                                     _u8 ece, _u8 cwr, _u8 doff, _u16 urg_ptr, _u16 check, _u8 res1)
{
    TCP_h.source   = source;
    TCP_h.dest	   = dest;
    TCP_h.seq      = seq;
    TCP_h.ack_seq  = ack_seq;
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
    TCP_h.window   = window;
    TCP_h.check    = check;
    TCP_h.urg_ptr  = urg_ptr;
}

void TCP_RawSocket::build_TCP_header(const tcphdr &tcp)
{
    TCP_h = tcp;
}
    
void TCP_RawSocket::set_TCP_options(const char *opt, size_t len)
{
    TCP_opt.assign(opt, len);
}

void TCP_RawSocket::set_TCP_options(const std::string& opt)
{
    TCP_opt = opt;
}

void TCP_RawSocket::adjust_TCP_csum(in_addr_t source, in_addr_t dest)
{
    TCP_h.check = 0;
    TCP_h.check = tcp_checksum(TCP_h, source, dest, data_payload, TCP_opt);
}

void TCP_RawSocket::adjust_TCP_doff()
{
    TCP_h.doff = (TCPSIZE + TCP_opt.size())/4;
}

void TCP_RawSocket::adjust_TCP_all(in_addr_t source, in_addr_t dest)
{
    adjust_TCP_doff();
    adjust_TCP_csum(source, dest);
}

void TCP_RawSocket::get_TCP_header(tcphdr &tcp)
{
    tcp = rcvd_TCP_h;
}

size_t TCP_RawSocket::get_TCP_options(char *opt,size_t len)
{
    int n = std::min(len, rcvd_TCP_opt.size());
    memcpy(opt, rcvd_TCP_opt.c_str(), n);
    return n;
}

size_t TCP_RawSocket::get_TCP_options(std::string &opt)
{
    opt = rcvd_TCP_opt;
    return opt.size();
}

void TCP_RawSocket::_build_packet(std::string &packet) const
{
    tcphdr tcp;
    _tcp_convert(TCP_h, tcp);
    packet.assign((char *)&tcp, TCPSIZE);
    packet += TCP_opt;
    packet += data_payload;
}

void TCP_RawSocket::_set_fields(const std::string &packet)
{
    iphdr *ip = (iphdr *)packet.c_str();
    _tcp_convert(*(tcphdr *)(packet.c_str() +ip->ihl*4), rcvd_TCP_h);
    rcvd_TCP_opt = packet.substr(ip->ihl*4 +TCPSIZE, rcvd_TCP_h.doff*4 -TCPSIZE);
    rcvd_data_payload = packet.substr(ip->ihl*4 +rcvd_TCP_h.doff*4);
}

TCP_IP_RawSocket::TCP_IP_RawSocket() : RawSocket(ipproto_tcp)
{
    setsockopt(sol_ip, ip_hdrincl, 1);
}

void TCP_IP_RawSocket::adjust_TCP_csum()
{
    TCP_RawSocket::adjust_TCP_csum(IP_h.saddr, IP_h.daddr);
}

void TCP_IP_RawSocket::adjust_TCP_all()
{
    adjust_TCP_doff();
    adjust_TCP_csum();
}

void TCP_IP_RawSocket::adjust_TCP_IP_all()
{
    adjust_TCP_all();
    adjust_IP_all();
}

void TCP_IP_RawSocket::_build_packet(std::string &packet) const
{
    iphdr ip;
    tcphdr tcp;
    _ip_convert(IP_h, ip);
    _tcp_convert(TCP_h, tcp);
    packet.assign((char *)&ip, IPSIZE);
    packet += IP_opt;
    packet.append((char *)&tcp, TCPSIZE);
    packet += TCP_opt;
    packet += data_payload;
}

void TCP_IP_RawSocket::_set_fields(const std::string &packet)
{
    _ip_convert(*(iphdr *)packet.c_str(), rcvd_IP_h);
    rcvd_IP_opt = packet.substr(IPSIZE, rcvd_IP_h.ihl*4 -IPSIZE);
    _tcp_convert(*(tcphdr *)(packet.c_str() + rcvd_IP_h.ihl*4), rcvd_TCP_h);
    rcvd_TCP_opt = packet.substr(rcvd_IP_h.ihl*4 +TCPSIZE, rcvd_TCP_h.doff*4 -TCPSIZE);
    rcvd_data_payload = packet.substr(rcvd_IP_h.ihl*4 +rcvd_TCP_h.doff*4);
}

void TCP_IP_RawSocket::build_IP_header
(_u32 saddr,_u32 daddr,_u8 ttl,_u8 version,_u8 protocol,_u16 id,_u8 tos,_u16 frag_off,_u8 ihl,_u16 tot_len,_u16 check)
{
    IP_RawSocket::build_IP_header(saddr,daddr,ttl,version,protocol,id,tos,frag_off,ihl,tot_len,check);
}

void ICMP_IP_RawSocket::build_IP_header
(_u32 saddr,_u32 daddr,_u8 ttl,_u8 version,_u8 protocol,_u16 id,_u8 tos,_u16 frag_off,_u8 ihl,_u16 tot_len,_u16 check)
{
    IP_RawSocket::build_IP_header(saddr,daddr,ttl,version,protocol,id,tos,frag_off,ihl,tot_len,check);
}

void UDP_IP_RawSocket::build_IP_header
(_u32 saddr,_u32 daddr,_u8 ttl,_u8 version,_u8 protocol,_u16 id,_u8 tos,_u16 frag_off,_u8 ihl,_u16 tot_len,_u16 check)
{
    IP_RawSocket::build_IP_header(saddr,daddr,ttl,version,protocol,id,tos,frag_off,ihl,tot_len,check);
}

_u16 IP_RawSocket::ip_checksum
(const struct iphdr &ip, const void *payload, size_t psize, const void *ip_opt, size_t ip_opt_len)
{
    std::string buf;
    struct iphdr _ip;
    _ip_convert(ip, _ip);
    buf.assign((char *)&_ip, IPSIZE);
    buf.append((char *)ip_opt, ip_opt_len);
    buf.append((char *)payload, psize);
    return _checksum(buf);
}

_u16 TCP_IP_RawSocket::ip_checksum
(const struct iphdr &ip, const struct tcphdr &tcp, const void *payload, size_t psize, 
 const void *ip_opt, size_t ip_opt_len, const void *tcp_opt, size_t tcp_opt_len)
{
    std::string buf;
    struct iphdr _ip;
    struct tcphdr _tcp;
    _ip_convert(ip, _ip);
    _tcp_convert(tcp, _tcp);
    buf.assign((char *)&_ip, IPSIZE);
    buf.append((char *)ip_opt, ip_opt_len);
    buf.append((char *)&_tcp, TCPSIZE);
    buf.append((char *)tcp_opt, tcp_opt_len);
    buf.append((char *)payload, psize);
    return _checksum(buf);
}

_u16 UDP_IP_RawSocket::ip_checksum
(const struct iphdr &ip, const struct udphdr &udp, const void *payload, size_t psize,
 const void *ip_opt, size_t ip_opt_len)
{
    std::string buf;
    struct iphdr _ip;
    struct udphdr _udp;
    _ip_convert(ip, _ip);
    _udp_convert(udp, _udp);
    buf.assign((char *)&_ip, IPSIZE);
    buf.append((char *)ip_opt, ip_opt_len);
    buf.append((char *)&_udp, UDPSIZE);
    buf.append((char *)payload, psize);
    return _checksum(buf);
}

_u16 ICMP_IP_RawSocket::ip_checksum
(const struct iphdr &ip, const struct icmphdr &icmp, const void *payload, size_t psize, 
 const void *ip_opt, size_t ip_opt_len)
{
    std::string buf;
    struct iphdr _ip;
    struct icmphdr _icmp;
    _ip_convert(ip, _ip);
    _icmp_convert(icmp, _icmp);
    buf.assign((char *)&_ip, IPSIZE);
    buf.append((char *)ip_opt, ip_opt_len);
    buf.append((char *)&_icmp, ICMPSIZE);
    buf.append((char *)payload, psize);
    return _checksum(buf);
}

_u16 TCP_RawSocket::tcp_checksum
(const struct tcphdr &tcp, in_addr_t saddr, in_addr_t daddr, const void *payload, 
 size_t psize, const void *tcp_opt, size_t tcp_opt_len)
{
    std::string buf;
    struct tcphdr _tcp;
    _tcp_convert(tcp, _tcp);
    buf.assign((char *)&_tcp, TCPSIZE);
    buf.append((char *)tcp_opt, tcp_opt_len);
    buf.append((char *)payload, psize);
    return _pseudo_checksum(buf, saddr, daddr, IPPROTO_TCP);
}

_u16 UDP_RawSocket::udp_checksum
(const struct udphdr &udp, in_addr_t saddr, in_addr_t daddr, const void *payload, size_t psize)
{
    std::string buf;
    struct udphdr _udp;
    _udp_convert(udp, _udp);
    buf.assign((char *)&_udp, UDPSIZE);
    buf.append((char *)payload, psize);
    return _pseudo_checksum(buf, saddr, daddr, IPPROTO_UDP);
}

_u16 ICMP_RawSocket::icmp_checksum(const struct icmphdr &icmp, const void *payload, size_t psize)
{
    std::string buf;
    struct icmphdr _icmp;
    _icmp_convert(icmp, _icmp);
    buf.assign((char *)&_icmp, ICMPSIZE);
    buf.append((char *)payload, psize);
    return _checksum(buf);
}

_u16 RawSocket::_checksum(const void *buf, size_t len)
{
    register _u32 sum=0;
    _u16 *ubuf = (_u16 *)buf; 

    if(len & 1)
        sum += ((char *)buf)[len-1];
    len >>= 1;
    while(len-- > 0) 
        sum += *ubuf++;
    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);
    return ~sum;
}

_u16 RawSocket::_checksum(const std::string &buf)
{
    return _checksum(buf.c_str(),buf.size());
}

_u16 RawSocket::_pseudo_checksum(const void *buf, size_t len, _u32 saddr, _u32 daddr, _u16 proto)
{
    register _u32 sum=0;
    _u16 *src = (_u16 *)&saddr, *dst = (_u16 *)&daddr;
    saddr = ::htonl(saddr);
    daddr = ::htonl(daddr);

    sum += src[0];
    sum += src[1];
    sum += dst[0];
    sum += dst[1];
    sum += htons(proto);
    sum += htons(len);

    if(len & 1)
        sum += ((char *)buf)[len-1];
    len >>= 1;
    _u16 *ubuf = (_u16 *)buf;
    while(len-- > 0) 
        sum += *ubuf++;
    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);
    return ~sum;
}

_u16 RawSocket::_pseudo_checksum(const std::string &buf, _u32 saddr, _u32 daddr, _u16 proto)
{
    return _pseudo_checksum(buf.c_str(), buf.size(), saddr, daddr, proto);
}

void TCP_RawSocket::_tcp_convert(const struct tcphdr &src, struct tcphdr &dst)
{
    dst.source   = ::htons(src.source);
    dst.dest	 = ::htons(src.dest);
    dst.seq      = ::htonl(src.seq);
    dst.ack_seq  = ::htonl(src.ack_seq);
    dst.res1     = src.res1;
    dst.doff     = src.doff;
    dst.fin      = src.fin;
    dst.syn      = src.syn;
    dst.rst      = src.rst;
    dst.psh      = src.psh;
    dst.ack      = src.ack;
    dst.urg      = src.urg;
    dst.ece      = src.ece;
    dst.cwr      = src.cwr;
    dst.window   = ::htons(src.window);
    dst.check    = src.check;
    dst.urg_ptr  = ::htons(src.urg_ptr);
}

void ICMP_RawSocket::_icmp_convert(const struct icmphdr &src, struct icmphdr &dst)
{
    dst.type     = src.type;
    dst.code	 = src.code;
    dst.check    = src.check;			
    dst.id	 = ::htons(src.id);
    dst.sequence = ::htons(src.sequence);
}

void UDP_RawSocket::_udp_convert(const struct udphdr &src, struct udphdr &dst)
{
    dst.source = ::htons(src.source);
    dst.dest   = ::htons(src.dest);
    dst.len    = ::htons(src.len);
    dst.check  = src.check;
}

void IP_RawSocket::_ip_convert(const struct iphdr &src, struct iphdr &dst)
{
    dst.ihl	 = src.ihl;
    dst.version  = src.version;
    dst.tos	 = src.tos;
    dst.tot_len  = ::ntohs(src.tot_len);
    dst.id	 = ::ntohs(src.id);
    dst.frag_off = ::ntohs(src.frag_off);
    dst.ttl	 = src.ttl;
    dst.protocol = src.protocol;
    dst.check	 = src.check;  	 
    dst.saddr	 = ::ntohl(src.saddr);
    dst.daddr	 = ::ntohl(src.daddr);
}

_u16 IP_RawSocket::ip_checksum(const struct iphdr &ip, const std::string& payload, const std::string &ip_opt)
{
    return ip_checksum(ip, payload.c_str(), payload.size(), ip_opt.c_str(), ip_opt.size());
}

_u16 TCP_IP_RawSocket::ip_checksum
(const struct iphdr &ip, const struct tcphdr &tcp, const std::string &payload,
 const std::string &ip_opt, const std::string &tcp_opt)
{
    return ip_checksum(ip,tcp,payload.c_str(),payload.size(),ip_opt.c_str(),ip_opt.size(),tcp_opt.c_str(),tcp_opt.size());
}

_u16 UDP_IP_RawSocket::ip_checksum
(const struct iphdr &ip, const struct udphdr &udp, const std::string &payload, const std::string &ip_opt)
{
    return ip_checksum(ip,udp,payload.c_str(),payload.size(),ip_opt.c_str(),ip_opt.size());
}

_u16 ICMP_IP_RawSocket::ip_checksum
(const struct iphdr &ip, const struct icmphdr &icmp, const std::string &payload, const std::string &ip_opt)
{
    return ip_checksum(ip,icmp,payload.c_str(),payload.size(),ip_opt.c_str(),ip_opt.size());
}

_u16 TCP_RawSocket::tcp_checksum
(const struct tcphdr &tcp, in_addr_t saddr, in_addr_t daddr, const std::string &payload, const std::string &tcp_opt)
{
    return tcp_checksum(tcp,saddr,daddr,payload.c_str(),payload.size(),tcp_opt.c_str(),tcp_opt.size());
}

_u16 UDP_RawSocket::udp_checksum
(const struct udphdr &udp, in_addr_t saddr, in_addr_t daddr, const std::string &payload)
{
    return udp_checksum(udp,saddr,daddr,payload.c_str(),payload.size());
}

_u16 ICMP_RawSocket::icmp_checksum(const struct icmphdr &icmp, const std::string &payload)
{
    return icmp_checksum(icmp, payload.c_str(),payload.size());
}

};
