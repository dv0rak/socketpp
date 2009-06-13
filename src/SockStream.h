#ifndef __SOCKSTREAM_H
#define __SOCKSTREAM_H 1

#include "Socket.h"
#include <streambuf>
#include <iostream>

namespace socketpp {

///@brief	publicly derived from std::streambuf and BaseSocket
class SockBuf : public std::streambuf, public BaseSocket {
public:
    static const int BUFSIZE = BUFSIZ;

    SockBuf() { _initBuf(); }
    ///@brief	calls BaseSocket(s)
    SockBuf(const Socket &s) : BaseSocket(s) { _initBuf(); }
    ///@brief	calls BaseSocket(t,prot)
    explicit SockBuf(type t, protocol prot=ipproto_ip) : BaseSocket(t, prot) { _initBuf(); } 
    ///@brief	calls BaseSocket(sd)
    SockBuf(int sd) : BaseSocket(sd) { _initBuf(); }
    
    ///@brief	closes socket descriptor and flushes output buffer
    void close();
    ///@brief	calls close()
    ~SockBuf() throw();

    using BaseSocket::shutdown;

protected:
    int overflow(int c = EOF);
    int underflow();
    int sync();

private:
    char _inBuf[BUFSIZE], _outBuf[BUFSIZE];
    
    void _initBuf();
};    

///@brief inherits from std::iostream, it allows the C++ stream approach with sockets
class SockStream : public std::iostream {
public:
    SockStream() : std::iostream(new SockBuf())
    {
        exceptions(badbit);
        free = true;
    }
    ///@brief	calls std::iostream(&s)
    explicit SockStream(SockBuf &s) : std::iostream(&s)
    {
        exceptions(badbit);
        free = false;
    }
    ///@brief	calls std::iostream(new SockBuf(s))
    explicit SockStream(const Socket &s) : std::iostream(new SockBuf(s))
    {
        exceptions(badbit);
        free = true;
    }
    ///@brief	calls std::iostream(new SockBuf(t,prot))
    explicit SockStream(type t, protocol prot=ipproto_ip) : std::iostream(new SockBuf(t,prot))
    {
        exceptions(badbit);
        free = true;
    }
 
    ///@brief	returns pointer to internal SockBuf object
    SockBuf* sockbuf() const { return (SockBuf *)rdbuf(); }
    ///@brief	operates on internal SockBuf object
    SockBuf* operator->() { return sockbuf(); }

    ~SockStream() { if(free) delete sockbuf(); }

private:
    bool free;
};

///@brief SockStream manipulator which writes "\r\n"
inline std::ostream &crlf (std::ostream &s)
{
    return s << "\r\n";
}

};

#endif
