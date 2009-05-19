#include "BaseSocket.h"

namespace socketpp {

void BaseSocket::_setBlocking(bool yes)
{
    long flags = ::fcntl(_sd, F_GETFL, 0);
    if(flags == -1) {
        throw error("_setBlocking", errno, "fcntl");
    }
    if(::fcntl(_sd, F_SETFL, (yes? flags&~O_NONBLOCK : flags|O_NONBLOCK)) == -1) {
        throw error("_setBlocking", errno, "fcntl");
    }
}

void BaseSocket::settimeout(double time)
{
    _timeout = time;
}

BaseSocket::BaseSocket()
{
    _timeout = 0.0;
    _sd        = -1;
}

BaseSocket::BaseSocket(type t, protocol prot)
{
    open(t,prot);
}

void BaseSocket::open(type t, protocol prot)
{
    _sd = ::socket(AF_INET,t,prot);
    if(_sd < 0) {
        throw error("BaseSocket",errno,"socket");
    }
}

BaseSocket::BaseSocket(int sd)
{
    _timeout = 0.0;
    _sd	= sd;
}

int BaseSocket::connect(const std::string& addr, port_t port)
{
    return connect(_h.inet_aton(_h.isIPv4(addr)? addr: _h.gethostbyname(addr)),port);
}

int BaseSocket::connect(in_addr_t addr, port_t port)
{
    int ret;
    struct sockaddr_in remote=__initaddr(htonl(addr),htons(port));
    if((ret=::connect(_sd,(struct sockaddr*)&remote,sizeof(remote))) < 0) {
        throw error("connect",errno,"connect");
    }
    return ret;
}

int BaseSocket::bind(const std::string& addr, port_t port)
{
    return bind(_h.inet_aton(_h.isIPv4(addr)? addr: _h.gethostbyname(addr)),port);
}

int BaseSocket::bind(in_addr_t addr, port_t port)
{
    int ret;
    struct sockaddr_in local=__initaddr(htonl(addr),htons(port));
    if((ret=::bind(_sd,(struct sockaddr*)&local,sizeof(local))) < 0) {
        throw error("bind",errno,"bind");
    }
    return ret;
}

void BaseSocket::close()
{
    ::close(_sd);
    _sd        = -1;
}

BaseSocket::~BaseSocket()
{
//  close();
}

std::string BaseSocket::remoteAddr()
{
    struct sockaddr_in remote;
    size_t len=sizeof(remote);
    if(::getpeername(_sd,(struct sockaddr*)&remote,&len) < 0) {
        throw error("remoteAddr",errno,"getpeername");
    }
    return _h.inet_ntoa(::ntohl(remote.sin_addr.s_addr));
}

port_t BaseSocket::remotePort()
{
    struct sockaddr_in remote;
    size_t len=sizeof(remote);
    if(::getpeername(_sd,(struct sockaddr*)&remote,&len) < 0) {
        throw error("remotePort",errno,"getpeername");
    }
    return ::ntohs(remote.sin_port);
}

std::string BaseSocket::localAddr()
{
    struct sockaddr_in local;
    size_t len=sizeof(local);
    if(::getsockname(_sd,(struct sockaddr*)&local,&len) < 0) {
        throw error("localAddr",errno,"getsockname");
    }
    return _h.inet_ntoa(::ntohl(local.sin_addr.s_addr));
}

port_t BaseSocket::localPort()
{
    struct sockaddr_in local;
    size_t len=sizeof(local);
    if(::getsockname(_sd,(struct sockaddr*)&local,&len) < 0) {
        throw error("localPort",errno,"getsockname");
    }
    return ::ntohs(local.sin_port);
}

struct sockaddr_in BaseSocket::__initaddr(in_addr_t addr, port_t port)
{
    struct sockaddr_in in;
    in.sin_family=AF_INET;
    in.sin_port=port;
    in.sin_addr.s_addr=addr;
    return in;
}

BaseSocket::BaseSocket(const BaseSocket &s)
{
    _timeout    = s._timeout;
    _h		= s._h;
    _sd		= s._sd;
}

int BaseSocket::connect(const std::string& addr, const std::string& serv, const char *prot)
{
    return connect(addr, _h.getservbyname(serv, prot));
}

int BaseSocket::connect(in_addr_t addr, const std::string& serv, const char *prot)
{
    return connect(addr, _h.getservbyname(serv, prot));
}

int BaseSocket::bind(const std::string& addr, const std::string& serv, const char *prot)
{
    return bind(addr, _h.getservbyname(serv,prot));
}    

int BaseSocket::bind(in_addr_t addr, const std::string& serv, const char *prot)
{
    return bind(addr, _h.getservbyname(serv,prot));
}

size_t BaseSocket::send(const char buf[], size_t size)
{
    int n, ret=0;
    while(size > 0) {
        if((n=::write(_sd,buf+ret,size)) < 0)
            throw error("send",errno,"write");
        size -= n;
        ret += n;
    }
    return ret;
}

size_t BaseSocket::send(const std::string &buf)
{
    return send(buf.c_str(),buf.size());
}

size_t BaseSocket::recv(char buf[], size_t size)
{
    int n;
    if((n=::read(_sd,buf,size)) < 0)
        throw error("recv",errno,"read");
    return n;
}

size_t BaseSocket::recv(std::string& buf, size_t size)
{
    char *nbuf = new char[size];
    int n=recv(nbuf,size);
    buf.assign(nbuf,n);
    delete[] nbuf;
    return n;
}

size_t BaseSocket::sendto(const char buf[], size_t size, in_addr_t addr, port_t port)
{
    int n, ret=0;
    struct sockaddr_in remote=__initaddr(htonl(addr),htons(port));
    while(size > 0) {
        if((n=::sendto(_sd,buf+ret,size,0,(struct sockaddr*)&remote,sizeof(remote))) < 0)
            throw error("sendto",errno,"sendto");
        size -= n;
        ret += n;
    }
    return ret;
}

size_t BaseSocket::sendto(const std::string& buf, in_addr_t addr, port_t port)
{
    return sendto(buf.c_str(), buf.size(), addr,port);
}

size_t BaseSocket::sendto(const char buf[], size_t size, const std::string& addr, port_t port)
{
    in_addr_t in;
    in=_h.inet_aton(_h.isIPv4(addr)? addr: _h.gethostbyname(addr));
    return sendto(buf,size,in,port);
}

size_t BaseSocket::sendto(const std::string& buf, const std::string& addr, port_t port)
{
    return sendto(buf.c_str(), buf.size(), addr,port);
}

size_t BaseSocket::sendto
(const char buf[],size_t size,in_addr_t addr,const std::string& serv,const char *prot)
{
    return sendto(buf, size, addr, _h.getservbyname(serv,prot));
}

size_t BaseSocket::sendto
(const std::string& buf, in_addr_t addr,const std::string& serv,const char *prot)
{
    return sendto(buf.c_str(), buf.size(), addr, _h.getservbyname(serv,prot));
}

size_t BaseSocket::sendto
(const char buf[],size_t size,const std::string& addr,const std::string& serv,const char *prot)
{
    return sendto(buf, size, addr, _h.getservbyname(serv,prot));
}

size_t BaseSocket::sendto
(const std::string& buf, const std::string& addr,const std::string& serv,const char *prot)
{
    return sendto(buf.c_str(),buf.size(),addr,serv,prot);
}

size_t BaseSocket::recvfrom(char buf[], size_t size, in_addr_t& addr, port_t& port)
{
    int n;
    struct sockaddr_in remote;
    size_t slen=sizeof(remote);
    if((n=::recvfrom(_sd,buf,size,0,(struct sockaddr*)&remote,&slen)) < 0) {
        throw error("recvfrom",errno,"recvfrom");
    }
    addr=::ntohl(remote.sin_addr.s_addr);
    port=::ntohs(remote.sin_port);
    return n;
}

size_t BaseSocket::recvfrom(std::string &buf, size_t size, in_addr_t& addr, port_t& port)
{
    char *nbuf = new char[size];
    int n=recvfrom(nbuf,size,addr,port);
    buf.assign(nbuf,n);
    delete[] nbuf;
    return n;
}

size_t BaseSocket::recvfrom(char buf[], size_t size, std::string& addr, port_t& port)
{
    in_addr_t in;
    size_t n=recvfrom(buf,size,in,port);
    addr=_h.inet_ntoa(in);
    return n;
}

size_t BaseSocket::recvfrom(std::string &buf, size_t size, std::string& addr, port_t& port)
{
    char *nbuf = new char[size];
    int n=recvfrom(nbuf,size,addr,port);
    buf.assign(nbuf,n);
    delete[] nbuf;
    return n;
} 

size_t BaseSocket::recvfrom(char buf[], size_t size, in_addr_t& addr)
{
    port_t p;
    return recvfrom(buf,size,addr,p);
}

size_t BaseSocket::recvfrom(std::string &buf, size_t size, in_addr_t& addr)
{
    char *nbuf = new char[size];
    int n=recvfrom(nbuf,size,addr);
    buf.assign(nbuf,n);
    delete[] nbuf;
    return n;
}

size_t BaseSocket::recvfrom(char buf[], size_t size, std::string& addr)
{
    port_t p;
    return recvfrom(buf,size,addr,p);
}

size_t BaseSocket::recvfrom(std::string &buf, size_t size, std::string& addr)
{
    char *nbuf = new char[size];
    int n=recvfrom(nbuf,size,addr);
    buf.assign(nbuf,n);
    delete[] nbuf;
    return n;
}

};
