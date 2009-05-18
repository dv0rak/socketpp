#include "AddrHandler.h"

namespace socketpp {

std::string AddrHandler::gethostbyname(const std::string& name)
{
    std::map<std::string,std::string>::iterator i=resolved.find(name);
    if(i!= resolved.end()) {
        return i->second;
    }
    struct hostent *he = ::gethostbyname(name.c_str());
    if(he == NULL) {
        throw SockException("gethostbyname",::hstrerror(h_errno),"gethostbyname");
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
        throw SockException("gethostbyaddr",::hstrerror(h_errno),"gethostbyaddr");
    }
    resolved[he->h_name] = addr;
    return he->h_name;
}

bool AddrHandler::isIPv4(const std::string& str)

{
    char c;
    int tok[4];
    if(::sscanf(str.c_str(),"%3d.%3d.%3d.%3d%c",&tok[0],&tok[1],&tok[2],&tok[3],&c) != 4)
        return false;
    for(int i=0; i<4; i++)
        if(tok[i]<0 || tok[i]>255) return false;
    return true;
}

in_addr_t AddrHandler::inet_aton(const std::string& str)
{
    if(!isIPv4(str)) {
        throw SockException("inet_aton","not a IPv4 dotted decimal address");
    }
    return ::ntohl(::inet_addr(str.c_str()));
}

std::string AddrHandler::inet_ntoa(in_addr_t addr)
{
    struct in_addr in;
    in.s_addr = ::htonl(addr);
    return ::inet_ntoa(in);
}

port_t AddrHandler::getServicePort(const std::string& name, const char *prot)
{
    servent *s;
    s = ::getservbyname(name.c_str(),prot);
    if(s == NULL) {
        throw SockException("getServicePort","an error occurred","getservbyname");
    }
    return ::ntohs(s->s_port);
}

std::string AddrHandler::getServiceName(port_t port, const char *prot)
{
    servent *s;
    s = ::getservbyport(::htons(port),prot);
    if(s == NULL) return "";
    return s->s_name;
}
};
