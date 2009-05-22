#ifndef __BASE_SOCKET_H
#define __BASE_SOCKET_H TRUE

#include "AddrHandler.h"
#include <fcntl.h>
#include <sys/select.h>
#include <string>

namespace socketpp {

///@brief Abstract base class
class BaseSocket {
public:
    ///@brief	opens socket descriptor
    ///@param   t	socket type
    ///@param	prot 	socket protocol
    void open(type t, protocol prot=ipproto_ip);
    ///@brief	closes socket descriptor
    virtual void close();

    ///@brief	sets/unsets blocking mode
    ///@param	yes	sets blocking mode if true, otherwise non-blocking one
    void setBlocking(bool yes);

    ///@brief 	gets options set on socket descriptor. Wrapper for getsockopt()
    ///@param   lev	getsockopt() level
    ///@param   opt	getsockopt() option type
    ///@param	optval	field filled by option value
    ///@return	getsockopt() return value
    template<typename T>
    int getsockopt(level lev, option opt, T& optval)
    {
        int ret;
        size_t siz = sizeof(T);
        if((ret=::getsockopt(_sd,lev,opt,&optval,&siz)) < 0)
            throw error("getsockopt",errno,"getsockopt");
        return ret;
    }

    ///@brief 	sets options set on socket descriptor. Wrapper for setsockopt()
    ///@param   lev	setsockopt() level
    ///@param   opt	setsockopt() option type
    ///@param	optval	option value
    ///@return	setsockopt() return value
    template<typename T>
    int setsockopt(level lev, option opt, const T& optval) 
    {
        int ret;
        if((ret=::setsockopt(_sd,lev,opt,&optval,sizeof(T))) < 0)
            throw error("setsockopt",errno,"setsockopt");
        return ret;
    }

    ///@brief connects socket to remote address
    ///@param addr	IPv4 dotted decimal address
    ///@param port	port number
    ///@return	connect() return value
    int connect(const std::string& addr, port_t port=0);
    ///@brief connects socket to remote address
    ///@param addr	IPv4 numerical address
    ///@param port	port number
    ///@return	connect() return value
    int connect(in_addr_t addr, port_t port=0);
    ///@brief connects socket to remote address
    ///@param addr	IPv4 dotted decimal address
    ///@param serv	service name
    ///@param prot	transport layer protocol
    ///@return	connect() return value
    int connect(const std::string& addr, const std::string& serv, const char *prot=NULL);
    ///@brief connects socket to remote address
    ///@param addr	IPv4 numerical address
    ///@param serv	service name
    ///@param prot	transport layer protocol
    ///@return	connect() return value
    int connect(in_addr_t addr, const std::string& serv, const char *prot=NULL);

    ///@brief	binds socket to local given address
    ///@param addr	IPv4 dotted decimal address
    ///@param port	port number
    ///@return	bind() return value
    int bind(const std::string& addr, port_t port=0);
    ///@brief	binds socket to local given address
    ///@param addr	IPv4 numerical address
    ///@param port	port number
    ///@return	bind() return value
    int bind(in_addr_t addr, port_t port=0);
    ///@brief	binds socket to local given address
    ///@param addr	IPv4 dotted decimal address
    ///@param serv	service name
    ///@param prot	transport layer protocol
    ///@return	bind() return value
    int bind(const std::string& addr, const std::string& serv, const char *prot=NULL);
    ///@brief	binds socket to local given address
    ///@param addr	IPv4 numerical address
    ///@param serv	service name
    ///@param prot	transport layer protocol
    ///@return	bind() return value
    int bind(in_addr_t addr, const std::string& serv, const char *prot=NULL);

    ///@brief sets a timeout on the following IO operations. If time is 0 timeout is cancelled.
    ///@param	time	timeout in seconds
    void settimeout(double time);

    ///@brief returns remote address to which socket is connected
    ///@return IPv4 dotted decimal address
    std::string remoteAddr();
    ///@brief returns local address to which socket is bound
    ///@return IPv4 dotted decimal address
    std::string localAddr();
    ///@brief returns local port to which socket is bound
    ///@return port number
    port_t localPort();
    ///@brief returns remote port bound to which socket is connected
    ///@return port number
    port_t remotePort();

    ///@brief returns socket descriptor number
    ///@return descriptor number
    inline int fileno() const  { return _sd; }
    ///@brief calls fileno()
    operator int() const { return fileno(); }
    
    ///@brief cleans internal DNS cache
    inline void   cleanDnsCache() { _h.cleanDnsCache(); }
    ///@brief returns internal DNS cache
    ///@return DNS cache
    inline std::map<std::string,std::string> getDnsCache() { return _h.getDnsCache(); }

    ///@brief desctuctor which calls close()
    virtual ~BaseSocket();

protected:
    int _sd;
    AddrHandler _h;
    double _timeout;

    enum _select_mode {
        read,
        write,
        err
    };

    ///@brief	simply instantiates the class without setting the internal socket descriptor 
    BaseSocket();
    ///@brief copy constructor
    BaseSocket(const BaseSocket &s);
    ///@brief constructor which opens the socket descriptor calling ::socket(AF_INET, t, prot)
    ///@param	t	socket type
    ///@param	prot	socket protocol
    BaseSocket(type t, protocol prot=ipproto_ip);
    ///@brief	constructor which uses the given socket descriptor
    ///@param 	fd	file descriptor
    BaseSocket(int fd);

    int _select(_select_mode m);
    
    ///@brief	sends data over socket
    ///@param	buf	data buffer
    ///@param   size	size of data in bytes
    ///@return  number of bytes sent
    size_t send(const char buf[], size_t size);
    ///@brief	reads data on socket
    ///@param   buf	buffer to fill with data
    ///@param	size 	max number of bytes to read
    ///@return  number of bytes received
    size_t recv(char buf[], size_t size);
    ///@brief	sends data over socket
    ///@param	buf	data string
    ///@return  number of bytes sent
    size_t send(const std::string &buf);
    ///@brief	reads data on socket
    ///@param	buf	string to fill with data
    ///@param	size 	max number of bytes to read
    ///@return  number of bytes received
    size_t recv(std::string &buf, size_t size);

    ///@brief	sends data over socket
    ///@param	buf	data buffer
    ///@param   size	size of data in bytes
    ///@param	addr	numerical remote address
    ///@param	port	remote port
    ///@return  number of bytes sent
    size_t sendto(const char buf[], size_t size, in_addr_t addr, port_t port=0);
    ///@brief	sends data over socket
    ///@param	buf	data string
    ///@param	addr	numerical remote address
    ///@param	port	remote port
    ///@return  number of bytes sent
    size_t sendto(const std::string &buf, in_addr_t addr, port_t port=0);
    ///@brief	sends data over socket
    ///@param	buf	data buffer
    ///@param   size	size of data in bytes
    ///@param	addr	dotted decimal remote address
    ///@param	port	remote port
    ///@return  number of bytes sent
    size_t sendto(const char buf[], size_t size, const std::string& addr, port_t port=0);
    ///@brief	sends data over socket
    ///@param	buf	data string
    ///@param	addr	dotted decimal remote address
    ///@param	port	remote port
    ///@return  number of bytes sent
    size_t sendto(const std::string& buf, const std::string& addr, port_t port=0);
    ///@brief	sends data over socket
    ///@param	buf	data buffer
    ///@param   size	size of data in bytes
    ///@param	addr	numerical remote address
    ///@param	serv	service name
    ///@param	prot 	transport layer protocol
    ///@return  number of bytes sent
    size_t sendto
    (const char buf[],size_t size,in_addr_t addr,const std::string& serv,const char *prot=NULL);
    ///@brief	sends data over socket
    ///@param	buf	data string
    ///@param	addr	numerical remote address
    ///@param	serv	service name
    ///@param	prot 	transport layer protocol
    ///@return  number of bytes sent
    size_t sendto
    (const std::string& buf, in_addr_t addr,const std::string& serv,const char *prot=NULL);
    ///@brief	sends data over socket
    ///@param	buf	data buffer
    ///@param   size	size of data in bytes
    ///@param	addr	dotted decimal remote address
    ///@param	serv	service name
    ///@param	prot 	transport layer protocol
    ///@return  number of bytes sent
    size_t sendto
    (const char buf[],size_t size,const std::string& addr,const std::string& serv,const char *prot=NULL);
    ///@brief	sends data over socket
    ///@param	buf	data string
    ///@param	addr	dotted decimal remote address
    ///@param	serv	service name
    ///@param	prot 	transport layer protocol
    ///@return  number of bytes sent
    size_t sendto
    (const std::string& buf,const std::string& addr,const std::string& serv,const char *prot=NULL);

    ///@brief	reads data on socket
    ///@param   buf	buffer to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with numerical remote address
    ///@param	port	field filled with remote port
    ///@return  number of bytes received
    size_t recvfrom(char buf[], size_t size, in_addr_t& addr, port_t& port);
    ///@brief	reads data on socket
    ///@param	buf	string to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with numerical remote address
    ///@param	port	field filled with remote port
    ///@return  number of bytes received
    size_t recvfrom(std::string& buf, size_t size, in_addr_t& addr, port_t& port);
    ///@brief	reads data on socket
    ///@param   buf	buffer to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with dotted decimal remote address
    ///@param	port	field filled with remote port
    ///@return  number of bytes received
    size_t recvfrom(char buf[], size_t size, std::string& addr, port_t& port);
    ///@brief	reads data on socket
    ///@param	buf	string to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with dotted decimal remote address
    ///@param	port	field filled with remote port
    ///@return  number of bytes received
    size_t recvfrom(std::string& buf, size_t size, std::string& addr, port_t& port);
    ///@brief	reads data on socket
    ///@param   buf	buffer to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with numerical remote address
    ///@return  number of bytes received
    size_t recvfrom(char buf[], size_t size, in_addr_t& addr);
    ///@brief	reads data on socket
    ///@param	buf	string to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with numerical remote address
    ///@return  number of bytes received
    size_t recvfrom(std::string &buf, size_t size, in_addr_t& addr);
    ///@brief	reads data on socket
    ///@param   buf	buffer to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with dotted decimal remote address
    ///@return  number of bytes received
    size_t recvfrom(char buf[], size_t size, std::string& addr);
    ///@brief	reads data on socket
    ///@param	buf	string to fill with data
    ///@param	size 	max number of bytes to read
    ///@param	addr	field filled with dotted decimal remote address
    ///@return  number of bytes received
    size_t recvfrom(std::string &buf, size_t size, std::string& addr);

private:
    struct sockaddr_in __initaddr(in_addr_t, port_t);

};
};

#endif
