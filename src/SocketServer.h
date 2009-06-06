#ifndef __SOCKET_SERVER_H
#define __SOCKET_SERVER_H TRUE

#include "Socket.h"

namespace socketpp {

///@brief	TCP server class which can accept simultaneously multiple connections through multithreading
class SocketServer : public BaseSocket {
public:
    SocketServer() : BaseSocket() { }
    ///@brief	calls BaseSocket(t)
    SocketServer(type t) : BaseSocket(t) { }
    ///@brief copy constructor
    SocketServer(const SocketServer &s) : BaseSocket(s) { }

    ///@brief	binds socket and puts it in listening mode
    ///@param	t	socket type
    ///@param 	addr	numerical local address to bind
    ///@param	port	local port to bind
    ///@param	maxconn	max number of connections to accept
    SocketServer
    (type t,in_addr_t addr,port_t port,unsigned int maxconn=0);
    ///@brief	binds socket and puts it in listening mode
    ///@param	t	socket type
    ///@param 	addr	dotted decimal local address to bind
    ///@param	port	local port to bind
    ///@param	maxconn	max number of connections to accept
    SocketServer
    (type t,const std::string& addr,port_t port,unsigned int maxconn=0);
    ///@brief	binds socket and puts it in listening mode
    ///@param	t	socket type
    ///@param 	addr	numerical local address to bind
    ///@param	serv	local service to bind
    ///@param	prot	transport layer protocol
    ///@param	maxconn	max number of connections to accept
    SocketServer
    (type t,in_addr_t addr,const std::string& serv,const char *prot=NULL,unsigned int maxconn=0);
    ///@brief	binds socket and puts it in listening mode
    ///@param	t	socket type
    ///@param 	addr	dotted decimal local address to bind
    ///@param	serv	local service to bind
    ///@param	prot	transport layer protocol
    ///@param	maxconn	max number of connections to accept
    SocketServer
    (type t,const std::string& addr,const std::string& serv,const char *prot=NULL,unsigned int maxconn=0);
 
    ///@brief	Accepts connection request
    ///@return connected socket object
    Socket& accept();
    ///@brief	puts socket in listening mode
    ///@param	maxconn		max number of connections to accept
    int listen(int maxconn);

    ///@brief	puts server in multithreaded mode
    ///@param	f	function to call when a connection is accepted, it requires as arguments the connected socket and a void pointer
    ///@param   pt	pointer to arguments accepted by function f
    void threadClientHandle(void (*f)(Socket&, void *), void *pt);

};
};
#endif
