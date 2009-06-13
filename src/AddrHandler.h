#ifndef __ADDR_HANDLER_H
#define __ADDR_HANDLER_H 1

#include "socketpp_def.h"
#include "SockException.h"
#include <map>
#include <string>
#include <vector>

namespace socketpp {

///@brief Class which deals with several things, like dns resolving or address format conversion
class AddrHandler {
public:
    /// @brief	gets IP source address to reach given destination address, watching kernel routing table
    /// @param	to	destination address
    /// @return	IP numerical address
    in_addr_t getAddrByRoute(in_addr_t to);

    /// @brief   resolves given name through a DNS lookup
    /// @param   name  hostname to resolve
    /// @return  vector of IP addresses
    std::vector<std::string> gethostbyname(const std::string& name);
    /// @brief   tries to obtain an hostname from a IP address, through a reverse DNS lookup
    /// @param   addr	IP address
    /// @return  hostname, or the address itself if no name was found
    std::string gethostbyaddr(const std::string& addr);

    /// @brief   obtains port conventionally bound to given service name
    /// @param   name	service name
    /// @return  port number
    static port_t getservbyname(const std::string& name);
    /// @brief   obtains service name conventionally bound to given port
    /// @param   port 	port number
    /// @param   prot	transport layer protocol, default tcp
    /// @return  service name, or the port number itself if no name was found
    static std::string getservbyport(port_t port, gai_proto prot = tcp);

    /// @brief   checks whether given string is in IPv4 format 
    /// @param   str 	string to check
    /// @return  true or false
    static bool isIPv4(const std::string& str) throw();

    /// @brief   conversion from IPv4 string format to numerical one
    /// @param   str	IPv4 string
    /// @return  numerical address
    static in_addr_t inet_aton(const std::string& str);
    /// @brief   conversion from IPv4 numerical format to dotted decimal one
    /// @param   addr	numerical address
    /// @return  IPv4 dotted decimal string
    static std::string inet_ntoa(in_addr_t addr);

    /// @brief   cleans internal DNS cache
    void cleanDnsCache()
    {
        resolved.clear();
        reversed.clear();
    }
    /// @brief	 clean internal routing table cache
    void cleanRouteCache()
    {
        routeCache.clear();
    }

private:
    std::map<in_addr_t, in_addr_t> routeCache;
    std::map<std::string, std::vector<std::string> > resolved;
    std::map<std::string, std::string> reversed;
};

};

#endif
