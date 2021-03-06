#ifndef __RAWSOCKET_H
#define __RAWSOCKET_H 1

#include "Socket.h"

namespace socketpp {

/// @brief	abstract base raw socket class. This class and all its derived classes don't require to perform any byte order conversion
class RawSocket : public BaseSocket {
public:
    /// @brief	 constructor which calls BaseSocket(sock_raw, prot)
    /// @param	prot	Socket protocol
    explicit RawSocket(protocol prot) : BaseSocket(sock_raw, prot) { }

    /// @brief	builds the internal data payload to send
    /// @param	data	pointer to data payload buffer
    /// @param	len	deta length in bytes
    void build_data_payload(const char *data, size_t len);
    /// @brief	builds the internal data payload to send
    /// @param	buf	data payload string
    void build_data_payload(const std::string &buf);

    /// @brief	sends the internal built packet
    /// @param	addr	destination IP address
    /// @return	number of bytes sent
    size_t send_packet(in_addr_t addr);
    /// @brief	sends the internal built packet
    /// @param	addr	destination hostname or dotted decimal address
    /// @return	number of bytes sent
    size_t send_packet(const std::string& addr);
    /// @brief  sends the internal built packet on a connected socket
    /// @return	number of bytes sent
    size_t send_packet();

    /// @brief	reads the next packet on the socket
    /// @param	len	max number of bytes to read
    /// @param 	addr	string to fill in with sender's address
    /// @return	number of bytes red
    size_t read_packet(size_t len, std::string &addr);
    /// @brief	reads the next packet on the socket
    /// @param	len	max number of bytes to read
    /// @param 	addr	in_addr_t to fill in with sender's address
    /// @return	number of bytes red
    size_t read_packet(size_t len, in_addr_t &addr);
    /// @brief	reads the next packet on the connected socket
    /// @param	len	max number of bytes to read
    /// @return	number of bytes red
    size_t read_packet(size_t len);

    /// @brief	gets the received data payload
    /// @param	data	pointer to buffer to fill with data
    /// @param	len	max number of bytes to write on buffer
    /// @return	number of bytes written
    size_t get_data_payload(char *data,size_t len);
    /// @brief	gets the received data payload
    /// @param	data	string to fill with data
    /// @return	number of bytes written
    size_t get_data_payload(std::string &data);

protected:
    std::string data_payload;
    std::string rcvd_data_payload;

    virtual void _build_packet(std::string &packet) const = 0;
    virtual void _set_fields(const std::string &packet) = 0;
    
    static _u16 _checksum(const std::string&);
    static _u16 _checksum(const void *buf, size_t nchar);
    static _u16 _pseudo_checksum(const void*, size_t, _u32, _u32, _u16);
    static _u16 _pseudo_checksum(const std::string&, _u32, _u32, _u16);
};

/// @brief	IP raw socket, without transport-based protocol. ONLY SENDING METHODS WORK
class IP_RawSocket : virtual public RawSocket {
public:
    /// @brief	constructor which calls RawSocket(ipproto_raw)
    IP_RawSocket() : RawSocket(ipproto_raw) { }

    ///@brief	calculates IP header checksum of a IP packet
    ///@param	ip	IP header
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@param	ip_opt	pointer to IP options field
    ///@param	ip_opt_len	IP options length
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const void *payload=NULL, size_t psize=0,
                            const void *ip_opt=NULL, size_t ip_opt_len=0);
    ///@brief	calculates IP header checksum of a IP packet
    ///@param	ip	IP header
    ///@param	payload	data payload field
    ///@param	ip_opt	IP options field
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const std::string& payload, const std::string &ip_opt);

    /// @brief	builds the internal IP header to send
    /// @param	ip	IP header
    void build_IP_header(const iphdr &ip);
    /// @brief	builds the internal IP header to send
    virtual void build_IP_header(_u32 saddr, _u32 daddr, _u8 ttl=64, _u8 version=4, _u8 protocol=0, _u16 id=0,
                                _u8 tos=0, _u16 frag_off=0, _u8 ihl=0, _u16 tot_len=0, _u16 check=0);

    /// @brief	sets the IP options field
    /// @param	opt 	pointer to option field
    /// @param	len 	option length	
    void set_IP_options(const char *opt, size_t len);
    /// @brief	sets the IP options field
    /// @param	opt	option string
    void set_IP_options(const std::string& opt);

    /// @brief	sets IP checksum field
    virtual void adjust_IP_csum();
    /// @brief	sets IP "total length" field
    virtual void adjust_IP_tot_len();
    /// @brief	sets IP "header length" field
    void adjust_IP_ihl();
    /// @brief	calls adjust_IP_tot_len() && adjust_IP_ihl() && adjust_IP_csum()
    void adjust_IP_all();
    
    /// @brief  gets received IP header
    /// @param	ip ip header to fill
    void get_IP_header(iphdr &ip);

    /// @brief  gets received IP options
    /// @param	opt	pointer to buffer to fill
    /// @param	len	max number of bytes to write
    /// @return	bytes written
    size_t get_IP_options(char *opt,size_t len);
    /// @brief  gets received IP options
    /// @param	opt	string to fill
    /// @return bytes written
    size_t get_IP_options(std::string &opt);

protected:
    struct iphdr IP_h;
    struct iphdr rcvd_IP_h;
    std::string IP_opt;
    std::string rcvd_IP_opt;

    static void _ip_convert(const struct iphdr&, struct iphdr&);
    virtual void _build_packet(std::string &packet) const;
    virtual void _set_fields(const std::string &packet);
};

/// @brief	ICMP raw socket without IP header handling
class ICMP_RawSocket : virtual public RawSocket {
public:
    /// @brief 	constructor which calls RawSocket(ipproto_icmp)
    ICMP_RawSocket() : RawSocket(ipproto_icmp) { }

    ///@brief	calculates ICMP header checksum
    ///@param	icmp	ICMP header
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@return	checksum value
    static _u16 icmp_checksum(const struct icmphdr &icmp, const void *payload=NULL, size_t psize=0);
    ///@brief	calculates ICMP header checksum
    ///@param	icmp	ICMP header
    ///@param	payload	data payload field
    ///@return	checksum value
    static _u16 icmp_checksum(const struct icmphdr &icmp, const std::string &payload);

    /// @brief	builds the internal ICMP header to send
    /// @param	icmp	icmp header
    void build_ICMP_header(const icmphdr &icmp);
    /// @brief	builds the internal ICMP header to send
    void build_ICMP_header(_u8 type, _u8 code=0, _u16 id=0, _u16 sequence=0, _u16 check=0);

    /// @brief	sets the internal ICMP header checksum
    void adjust_ICMP_csum();

    /// @brief	gets the received ICMP header
    /// @param	icmp	ICMP header to modify
    void get_ICMP_header(icmphdr &icmp);

protected:
    icmphdr ICMP_h;
    icmphdr rcvd_ICMP_h;

    static void _icmp_convert(const struct icmphdr&, struct icmphdr&);
    virtual void _build_packet(std::string &packet) const;
    virtual void _set_fields(const std::string &packet);
    
};

/// @brief  ICMP raw socket with IP header handling.
class ICMP_IP_RawSocket : public IP_RawSocket, public ICMP_RawSocket {
public:
    /// @brief	  constructor which calls RawSocket(ipproto_udp) and setsockopt(ip_hdrincl)
    ICMP_IP_RawSocket();

    ///@brief	calculates IP header checksum of a ICMP packet
    ///@param	ip	IP header
    ///@param	icmp	ICMP header
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@param	ip_opt	pointer to IP options field
    ///@param	ip_opt_len	IP options length
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const struct icmphdr &icmp, const void *payload=NULL, size_t psize=0,
                            const void *ip_opt=NULL, size_t ip_opt_len=0);
    ///@brief	calculates IP header checksum of a ICMP packet
    ///@param	ip	IP header
    ///@param	icmp	ICMP header
    ///@param	payload	data payload field
    ///@param	ip_opt	IP options field
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const struct icmphdr &icmp, const std::string &payload, 
                            const std::string &ip_opt);

    /// @brief	builds the internal IP header to send
    void build_IP_header(_u32 saddr, _u32 daddr, _u8 ttl=64, _u8 version=4, _u8 protocol=IPPROTO_ICMP, _u16 id=0,
                         _u8 tos=0, _u16 frag_off=0, _u8 ihl=0, _u16 tot_len=0, _u16 check=0);

    /// @brief	sets IP checksum field
    void adjust_IP_csum();
    /// @brief	sets IP "total length" field
    void adjust_IP_tot_len();

    /// @brief	calls adjust_ICMP_all() && adjust_IP_all()
    void adjust_ICMP_IP_all();

protected:
    void _build_packet(std::string &packet) const;
    void _set_fields(const std::string &packet);
};

/// @brief    UDP raw socket without IP header handling
class UDP_RawSocket : virtual public RawSocket {
public:
    /// @brief	constructor which calls RawSocket(ipproto_udp)
    UDP_RawSocket() : RawSocket(ipproto_udp) { }

    ///@brief	calculates UDP header checksum
    ///@param	udp	UDP header
    ///@param	saddr	source address
    ///@param	daddr	destination address
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@return	checksum value
    static _u16 udp_checksum(const struct udphdr &udp, in_addr_t saddr, in_addr_t daddr, 
                             const void *payload=NULL, size_t psize=0);
    ///@brief	calculates UDP header checksum
    ///@param	udp	UDP header
    ///@param	saddr	source address
    ///@param	daddr	destination address
    ///@param	payload	data payload field
    ///@return	checksum value
    static _u16 udp_checksum(const struct udphdr &udp, in_addr_t saddr, in_addr_t daddr, const std::string &payload);

    /// @brief	builds the internal UDP header
    /// @param	source	   source port
    /// @param  dest	   destination port
    /// @param  len	   UDP+data total length in bytes
    /// @param  check	   UDP pseudo-checksum
    void build_UDP_header(_u16 source, _u16 dest, _u16 len=0, _u16 check=0);
    /// @brief	builds the internal UDP header
    /// @param  udp	   UDP header
    void build_UDP_header(const udphdr &udp);

    /// @brief	sets the internal UDP pseudo-header checksum
    /// @param  source	   IP source address
    /// @param	dest	   IP destination address
    void adjust_UDP_csum(in_addr_t source, in_addr_t dest);
    
    /// @brief	sets the internal header "len" field
    void adjust_UDP_len();
    /// @brief	calls adjust_UDP_len() and adjust_UDP_csum(source,dest)
    /// @param	source	   IP source address
    /// @param  dest	   IP destination address
    void adjust_UDP_all(in_addr_t source, in_addr_t dest);

    /// @brief	gets the received UDP header
    /// @param	udp	header to modify
    void get_UDP_header(udphdr &udp);

protected:
    udphdr UDP_h;
    udphdr rcvd_UDP_h;

    static void _udp_convert(const struct udphdr&, struct udphdr&);
    virtual void _build_packet(std::string &packet) const;
    virtual void _set_fields(const std::string &packet);
    
};

/// @brief	UDP raw socket with IP header handling. 
class UDP_IP_RawSocket : public UDP_RawSocket, public IP_RawSocket {
public:
    /// @brief	constructor which calls RawSocket(ipproto_udp) and setsockopt(ip_hdrincl)
    UDP_IP_RawSocket();

    /// @brief	builds the internal IP header to send
    void build_IP_header(_u32 saddr, _u32 daddr, _u8 ttl=64, _u8 version=4, _u8 protocol=IPPROTO_UDP, _u16 id=0,
                         _u8 tos=0, _u16 frag_off=0, _u8 ihl=0, _u16 tot_len=0, _u16 check=0);

    ///@brief	calculates IP header checksum of a UDP packet
    ///@param	ip	IP header
    ///@param	udp	udp header
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@param	ip_opt	pointer to IP options field
    ///@param	ip_opt_len	IP options length
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const struct udphdr &udp, const void *payload=NULL, size_t psize=0,
                            const void *ip_opt=NULL, size_t ip_opt_len=0);
    ///@brief	calculates IP header checksum of a UDP packet
    ///@param	ip	IP header
    ///@param	udp	udp header
    ///@param	payload	data payload field
    ///@param	ip_opt	IP options field
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const struct udphdr &udp, const std::string &payload, 
                            const std::string &ip_opt);

    /// @brief	sets IP checksum field
    void adjust_IP_csum();
    /// @brief	sets IP "total length" field
    void adjust_IP_tot_len();

    /// @brief	   sets UDP checksum, using source and dest IP addresses token by IP header
    void adjust_UDP_csum();
    /// @brief	   calls adjust_UDP_len() && adjust_UDP_csum()
    void adjust_UDP_all();
    /// @brief	   calls adjust_UDP_all() and adjust_IP_all()
    void adjust_UDP_IP_all();

protected:
    void _build_packet(std::string &packet) const ;
    void _set_fields(const std::string &packet);
    
};

/// @brief    TCP raw socket without IP header handling
class TCP_RawSocket : virtual public RawSocket {
public:
    /// @brief	constructor which calls RawSocket(ipproto_tcp)
    TCP_RawSocket() : RawSocket(ipproto_tcp) { }

    ///@brief	calculates TCP header checksum
    ///@param	tcp	TCP header
    ///@param	saddr	source address
    ///@param	daddr	destination address
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@param	tcp_opt	pointer to TCP options field
    ///@param	tcp_opt_len	TCP options length
    ///@return	checksum value
    static _u16 tcp_checksum(const struct tcphdr &tcp, in_addr_t saddr, in_addr_t daddr,
                             const void *payload=NULL, size_t psize=0, const void *tcp_opt=NULL, size_t tcp_opt_len=0);
    ///@brief	calculates TCP header checksum
    ///@param	tcp	TCP header
    ///@param	saddr	source address
    ///@param	daddr	destination address
    ///@param	payload	data payload field
    ///@param	tcp_opt	TCP options field
    ///@return	checksum value
    static _u16 tcp_checksum(const struct tcphdr &tcp, in_addr_t saddr, in_addr_t daddr,
                             const std::string &payload, const std::string &tcp_opt);

    /// @brief	builds the internal TCP header
    void build_TCP_header(_u16 source, _u16 dest, _u32 seq, _u32 ack_seq, _u16 window,
                          _u8 fin=0, _u8 syn=0, _u8 rst=0, _u8 psh=0, _u8 ack=0, _u8 urg=0,
                          _u8 ece=0, _u8 cwr=0, _u8 doff=0, _u16 urg_ptr=0, _u16 check=0, _u8 res1=0);
    /// @brief	builds the internal TCP header
    /// @param  tcp	   TCP header
    void build_TCP_header(const tcphdr &tcp);
    
    /// @brief	sets the TCP options field
    /// @param	opt 	pointer to option field
    /// @param	len 	option length	
    void set_TCP_options(const char *opt, size_t len);
    /// @brief	sets the TCP options field
    /// @param	opt	option string
    void set_TCP_options(const std::string& opt);

    /// @brief	sets the internal TCP pseudo-header checksum
    /// @param  source	   IP source address
    /// @param	dest	   IP destination address
    void adjust_TCP_csum(in_addr_t source, in_addr_t dest);
    /// @brief	sets the internal header "doff" field
    void adjust_TCP_doff();
    /// @brief	calls adjust_TCP_doff() && adjust_TCP_csum(source,dest)
    /// @param	source	   IP source address
    /// @param  dest	   IP destination address
    void adjust_TCP_all(in_addr_t source, in_addr_t dest);

    /// @brief	gets the received TCP header
    /// @param	tcp	header to modify
    void get_TCP_header(tcphdr &tcp);
    
    /// @brief  gets received TCP options
    /// @param	opt	pointer to buffer to fill
    /// @param	len	max number of bytes to write
    /// @return	bytes written
    size_t get_TCP_options(char *opt,size_t len);
    /// @brief  gets received TCP options
    /// @brief	string to fill
    /// @return	bytes written
    size_t get_TCP_options(std::string &opt);

protected:
    tcphdr TCP_h;
    tcphdr rcvd_TCP_h;
    std::string TCP_opt;
    std::string rcvd_TCP_opt;

    static void _tcp_convert(const struct tcphdr&, struct tcphdr&);
    virtual void _build_packet(std::string &packet) const;
    virtual void _set_fields(const std::string &packet);
};

/// @brief	TCP raw socket with IP header handling. 
class TCP_IP_RawSocket : public TCP_RawSocket, public IP_RawSocket {
public:
    /// @brief	constructor which calls RawSocket(ipproto_tcp) and setsockopt(ip_hdrincl)
    TCP_IP_RawSocket();

    /// @brief	builds the internal IP header to send
    void build_IP_header(_u32 saddr, _u32 daddr, _u8 ttl=64, _u8 version=4, _u8 protocol=IPPROTO_TCP, _u16 id=0,
                         _u8 tos=0, _u16 frag_off=0, _u8 ihl=0, _u16 tot_len=0, _u16 check=0);

    ///@brief	calculates IP header checksum of a TCP packet
    ///@param	ip	IP header
    ///@param	tcp	TCP header
    ///@param	payload	pointer to data payload field
    ///@param	psize	data payload size
    ///@param	ip_opt	pointer to IP options field
    ///@param	ip_opt_len	IP options length
    ///@param	tcp_opt	pointer to TCP options field
    ///@param	tcp_opt_len	TCP options length
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const struct tcphdr &tcp, const void *payload=NULL, size_t psize=0,
                            const void *ip_opt=NULL, size_t ip_opt_len=0, const void *tcp_opt=NULL, size_t tcp_opt_len=0);
    ///@brief	calculates IP header checksum of a TCP packet
    ///@param	ip	IP header
    ///@param	tcp	TCP header
    ///@param	payload	data payload field
    ///@param	ip_opt	IP options field
    ///@param	tcp_opt	TCP options field
    ///@return	checksum value
    static _u16 ip_checksum(const struct iphdr &ip, const struct tcphdr &tcp, const std::string &payload,
                            const std::string &ip_opt, const std::string &tcp_opt);

    /// @brief	sets IP checksum field
    void adjust_IP_csum();
    /// @brief	sets IP "total length" field
    void adjust_IP_tot_len();

    /// @brief	   sets TCP checksum, using source and dest IP addresses token by IP header
    void adjust_TCP_csum();
    /// @brief	   calls adjust_TCP_doff() && adjust_TCP_csum()
    void adjust_TCP_all();
    /// @brief	   calls adjust_TCP_all() && adjust_IP_all()
    void adjust_TCP_IP_all();

protected:
    void _build_packet(std::string &packet) const;
    void _set_fields(const std::string &packet);
};

};

#endif
