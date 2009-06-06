#include "AddrHandler.h"
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

namespace socketpp {

std::string AddrHandler::gethostbyname(const std::string& name)
{
    std::map<std::string,std::string>::iterator i=resolved.find(name);
    if(i!= resolved.end()) {
        return i->second;
    }
    struct hostent *he = ::gethostbyname(name.c_str());
    if(he == NULL) {
        throw h_error("gethostbyname", h_errno, "gethostbyname");
    }
    std::string addr=inet_ntoa(::ntohl((*(struct in_addr *)he->h_addr).s_addr));
    resolved[name]=addr;
    return addr;
}

std::string AddrHandler::gethostbyaddr(const std::string& addr)
{
    for(std::map<std::string,std::string>::iterator i=resolved.begin(); i!=resolved.end(); i++) {
        if(i->second==addr) {
            return i->first;
        }
    }
    struct in_addr in;
    in.s_addr = ::htonl(inet_aton(addr));
    struct hostent *he = ::gethostbyaddr(&in,sizeof in,AF_INET);
    if(he == NULL) {
        throw h_error("gethostbyaddr", h_errno, "gethostbyaddr");
    }
    resolved[he->h_name] = addr;
    return he->h_name;
}

bool AddrHandler::isIPv4(const std::string& str) throw()

{
    try { inet_aton(str); }
    catch(h_error &h) { return false; }
    return true;
}

in_addr_t AddrHandler::inet_aton(const std::string& str)
{
    in_addr_t addr;
    if(::inet_pton(AF_INET, str.c_str(), &addr) == 0) {
        throw h_error("inet_aton", "`"+ str+ "' not a IPv4 address");
    }
    return ::ntohl(addr);
}

std::string AddrHandler::inet_ntoa(in_addr_t addr) throw()
{
    struct in_addr in;
    in.s_addr = ::htonl(addr);
    return ::inet_ntoa(in);
}

port_t AddrHandler::getservbyname(const std::string& name, const char *prot)
{
    servent *s;
    s = ::getservbyname(name.c_str(),prot);
    if(s == NULL) {
        throw h_error("getservbyname", "error", "getservbyname");
    }
    return ::ntohs(s->s_port);
}

std::string AddrHandler::getservbyport(port_t port, const char *prot) throw()
{
    servent *s;
    s = ::getservbyport(::htons(port),prot);
    if(s == NULL) return "";
    return s->s_name;
}

// Taken from http://www.linuxjournal.com/article/8498
in_addr_t AddrHandler::getAddrByRoute(in_addr_t to)
{
    if(to == INADDR_LOOPBACK) {
        return INADDR_LOOPBACK;
    }


    // open socket
    int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if(fd < 0) {
        throw error("getAddrByRoute", errno, "socket");
    }

    // setup local address & bind using
    // this address
    struct sockaddr_nl la;
    bzero(&la, sizeof(la));
    la.nl_family = AF_NETLINK;
    la.nl_pid = ::getpid();
    if(::bind(fd, (struct sockaddr*) &la, sizeof(la)) < 0) {
        throw error("getAddrByRoute", errno, "bind");
    }


    struct nlmsghdr reqnl;
    memset(&reqnl, 0, sizeof(reqnl));
    struct rtmsg    reqrt;
    memset(&reqrt, 0, sizeof(reqrt));
    char reqbuf[8192];
    memset(&reqbuf, 0, sizeof(reqbuf));


    // set the NETLINK header
    reqnl.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    reqnl.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    reqnl.nlmsg_type = RTM_GETROUTE;

    // set the routing message header
    reqrt.rtm_family = AF_INET;
    reqrt.rtm_table = RT_TABLE_MAIN;




    // create the remote address
    // to communicate
    struct sockaddr_nl pa;
    bzero(&pa, sizeof(pa));
    pa.nl_family = AF_NETLINK;

    // initialize & create the struct msghdr supplied
    // to the sendmsg() function
    struct msghdr msg;
    bzero(&msg, sizeof(msg));
    msg.msg_name = (void *) &pa;
    msg.msg_namelen = sizeof(pa);

    // place the pointer & size of the RTNETLINK
    // message in the struct msghdr
    struct iovec iov;
    iov.iov_base = (void *) &reqnl;
    iov.iov_len = reqnl.nlmsg_len;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // send the RTNETLINK message to kernel
    int rtn;
    rtn = ::sendmsg(fd, &msg, 0);
    if(rtn < 0) {
        throw error("getAddrByRoute", errno, "sendmsg");
    }


    char *p;

    // initialize the socket read buffer
    char buf[8192];
    bzero(buf, sizeof(buf));

    p = buf;
    unsigned int nll = 0;

    // read from the socket until the NLMSG_DONE is
    // returned in the type of the RTNETLINK message
    // or if it was a monitoring socket
    struct nlmsghdr *nlp;
    while(1) {
        rtn = ::recv(fd, p, sizeof(buf) - nll, 0);
        if(rtn < 0) {
            throw error("getAddrByRoute", errno, "recv");
        }

        nlp = (struct nlmsghdr *) p;

        if(nlp->nlmsg_type == NLMSG_DONE)
            break;

        // increment the buffer pointer to place
        // next message
        p += rtn;

        // increment the total size by the size of
        // the last received message
        nll += rtn;

        if((la.nl_groups & RTMGRP_IPV4_ROUTE) == RTMGRP_IPV4_ROUTE)
            break;
    }
    ::close(fd);



    // outer loop: loops thru all the NETLINK
    // headers that also include the route entry
    // header
    struct rtmsg *rtp;
    int rtl;
    struct rtattr *rtap;

    nlp = (struct nlmsghdr *) buf;

    for(; NLMSG_OK(nlp, nll); nlp=NLMSG_NEXT(nlp, nll)) {
        // get route entry header
        rtp = (struct rtmsg *) NLMSG_DATA(nlp);

        // we are only concerned about the
        // main route table
        if(rtp->rtm_table != RT_TABLE_MAIN)
            continue;

        in_addr_t dstaddr = 0;
        int ifindex = 0;

        // inner loop: loop thru all the attributes of
        // one route entry
        rtap = (struct rtattr *) RTM_RTA(rtp);
        rtl = RTM_PAYLOAD(nlp);

        for(; RTA_OK(rtap, rtl); rtap=RTA_NEXT(rtap,rtl)) {
            switch(rtap->rta_type) {
                // destination IPv4 address
                case RTA_DST:
                dstaddr = ::htonl(*(in_addr_t *)RTA_DATA(rtap));
                break;

               // unique ID associated with the network
               // interface
               case RTA_OIF:
               ifindex = *(int *)RTA_DATA(rtap);
               break;
            }
        }
        in_addr_t mask = ~(0xffffffff >> rtp->rtm_dst_len);

        if((to & mask) == (dstaddr & mask)) {
            int sd = ::socket(AF_INET, SOCK_DGRAM, 0);
            if(sd < 0) {
                throw error("getAddrByRoute", errno, "socket");
            }
            struct ifreq ifr;
            ifr.ifr_ifindex = ifindex;

            if (ioctl(sd, SIOCGIFNAME, &ifr)<0 || ioctl(sd, SIOCGIFADDR, &ifr)<0) {
                throw error("getAddrByRoute", errno, "ioctl");
            }
            ::close(sd);
            struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
            return ::htonl(sin->sin_addr.s_addr);
        }
    }

    return -1;
}

};
