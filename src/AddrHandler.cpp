#include "AddrHandler.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace socketpp {

in_addr_t AddrHandler::getAddrByIface(const std::string &name)
{
    int sd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0) {
        throw error("getAddrByIface", errno, "socket");
    }
    struct ifreq ifr;
    strncpy((char *)ifr.ifr_name, name.c_str(), IFNAMSIZ);
    
    if(ioctl(sd, SIOCGIFINDEX, &ifr) < 0) {
        throw error("getAddrByIface", errno, "ioctl");
    }
    if(ioctl(sd, SIOCGIFADDR, &ifr) < 0) {
        throw error("getAddrByIface", errno, "ioctl");
    }
    ::close(sd);
    struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
    return ::htonl(sin->sin_addr.s_addr);
}

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
};
