#include "SocketServer.h"
#include <pthread.h>

struct thrdarg {
    void (*func)(socketpp::Socket&, void*);
    socketpp::Socket sock;
    void *pt;
};

static void * thread(void* a)
{
    thrdarg *arg = (thrdarg*)a;
    (arg->func)(arg->sock, arg->pt);
    delete arg;
    ::pthread_exit(NULL);
}

namespace socketpp {

void SocketServer::threadClientHandle(void (*func)(Socket&, void*), void *p)
{
    while(1) {
        pthread_t t;
        ::thrdarg *arg = new ::thrdarg;
        arg->sock = accept();
        arg->func = func;
        arg->pt   = p;
        ::pthread_create(&t,NULL,thread,arg);
        ::pthread_detach(t);
    }
}

Socket& SocketServer::accept()
{
    struct sockaddr remote;
    socklen_t slen = sizeof(remote);

    if(_timeout!=0.0 && _select(read)==0) 
        throw timeout("accept","timeout expired");

    int sd = ::accept(_sd,&remote,&slen);
    if(sd < 0) {
        throw error("accept",errno,"accept");
    }
    return *(new Socket(sd));
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
(type t,in_addr_t addr, const std::string& serv, const char *prot, unsigned int maxconn) : BaseSocket(t)
{
    bind(addr, serv, prot);
    if(maxconn) listen(maxconn);
}

SocketServer::SocketServer
(type t, const std::string& addr,const std::string& serv,const char *prot,unsigned int maxconn) : BaseSocket(t)
{
    bind(addr, serv, prot);
    if(maxconn) listen(maxconn);
}

};
