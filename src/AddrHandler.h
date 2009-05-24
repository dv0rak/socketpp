#ifndef __ADDR_HANDLER_H
#define __ADDR_HANDLER_H 1

#include "socketpp_def.h"
#include "SockException.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <map>
#include <string>
#include <regex.h>

namespace socketpp {

///@brief Class which deals with several things, like dns resolving or address format conversion
class AddrHandler {
public:
    /// @brief   resolves given name through a DNS lookup
    /// @param   name  hostname to resolve
    /// @return  IP address
    std::string gethostbyname(const std::string& name);
    /// @brief   tries to obtain an hostname from a IP address, through a reverse DNS lookup
    /// @param   addr	IP address
    /// @return  hostname
    std::string gethostbyaddr(const std::string& addr);

    /// @brief   obtains port conventionally bound to given service name
    /// @param   name	service name
    /// @param   prot	transport layer protocol
    /// @return  port number
    port_t getservbyname(const std::string& name, const char *prot=NULL);
    /// @brief   obtains service name conventionally bound to given port
    /// @param   port 	port number
    /// @param   prot	transport layer protocol
    /// @return  service name
    static std::string getservbyport(port_t port, const char *prot=NULL);

    /// @brief   checks whether given string is in IPv4 format 
    /// @param   str 	string to check
    /// @return  true or false
    static bool isIPv4(const std::string& str);

    /// @brief   conversion from IPv4 string format to numerical one
    /// @param   str	IPv4 string
    /// @return  numerical address
    static in_addr_t inet_aton(const std::string& str);
    /// @brief   conversion from IPv4 numerical format to dotted decimal one
    /// @param   addr	numerical address
    /// @return  IPv4 dotted decimal string
    static std::string inet_ntoa(in_addr_t addr);

    /// @brief   returns internal DNS cache
    /// @return  internal DNS cache
    inline std::map<std::string,std::string> getDnsCache() { return resolved; }
    /// @brief   cleans internal DNS cache
    inline void cleanDnsCache() { resolved.clear(); }
private:
    std::map<std::string, std::string> resolved;

};
};

#endif
