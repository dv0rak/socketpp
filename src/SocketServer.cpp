#include "SocketServer.h"
#include <pthread.h>

namespace socketpp {

void * SocketServer::_thread(void* a)
{
    _thrdarg *arg = (_thrdarg *)a;
    (arg->func)(arg->sock, arg->pt);
    delete arg;
    ::pthread_exit(NULL);
}

void SocketServer::threadClientHandle(void (*func)(Socket&, void*), void *p)
{
    while(1) {
        pthread_t t;
        _thrdarg *arg = new _thrdarg;
        arg->sock = accept();
        arg->func = func;
        arg->pt   = p;
        ::pthread_create(&t, NULL, _thread, arg);
        ::pthread_detach(t);
    }
}

Socket SocketServer::accept()
{
    struct sockaddr remote;
    socklen_t slen = sizeof(remote);

    if(_timeout != 0.0) { 
        int s = _select(read);
        if(s == 0) { 
            throw timeout("accept","timeout expired");
        }
        if(s < 0)
            throw error("accept",errno);
    }
    int sd = ::accept(_sd,&remote,&slen);
    if(sd < 0) {
        throw error("accept",errno,"accept");
    }
    return Socket(sd);
}

int SocketServer::listen(int maxconn)
{
    int ret;
    if((ret=::listen(_sd,maxconn)) < 0) {
        throw error("listen",errno,"listen");
    }
    return ret;
}

SocketServer::SocketServer
(type t,in_addr_t addr, port_t port, unsigned int maxconn) : BaseSocket(t)
{
    bind(addr, port);
    if(maxconn) listen(maxconn);
}

SocketServer::SocketServer
(type t,const std::string& addr, port_t port, unsigned int maxconn) : BaseSocket(t)
{
    bind(addr, port);
    if(maxconn) listen(maxconn);
}
    
SocketServer::SocketServer
(type t,in_addr_t addr, const std::string& serv, unsigned int maxconn) : BaseSocket(t)
{
    bind(addr, serv);
    if(maxconn) listen(maxconn);
}

SocketServer::SocketServer
(type t, const std::string& addr,const std::string& serv, unsigned int maxconn) : BaseSocket(t)
{
    bind(addr, serv);
    if(maxconn) listen(maxconn);
}

};
