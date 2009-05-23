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

bool AddrHandler::isIPv4(const std::string& str)

{
    regex_t re;
    const char *pattern = "^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}"\
                          "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$";
 
    if(::regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return false; 
    if(::regexec(&re, str.c_str(), (size_t)0, NULL, 0) != 0) return false;
    ::regfree(&re);
    return true;
}

in_addr_t AddrHandler::inet_aton(const std::string& str)
{
    if(!isIPv4(str)) {
        throw h_error("inet_aton", "`"+ str+ "' not a IPv4 dotted decimal address");
    }
    return ::ntohl(::inet_addr(str.c_str()));
}

std::string AddrHandler::inet_ntoa(in_addr_t addr)
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

std::string AddrHandler::getservbyport(port_t port, const char *prot)
{
    servent *s;
    s = ::getservbyport(::htons(port),prot);
    if(s == NULL) return "";
    return s->s_name;
}
};
