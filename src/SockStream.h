#ifndef __SOCKSTREAM_H
#define __SOCKSTREAM_H 1

#include "Socket.h"
#include <streambuf>
#include <iostream>

namespace socketpp {

///@brief	publicly derived from std::streambuf and BaseSocket
class SockBuf : public std::streambuf, public BaseSocket {
public:
    static const int BUFSIZE=BUFSIZ;

    SockBuf() : BaseSocket()
        { __initBuf(); }
    ///@brief	calls BaseSocket(s)
    SockBuf(const Socket &s) : BaseSocket(s)
        { __initBuf(); }
    ///@brief	calls BaseSocket(t,prot)
    SockBuf(type t, protocol prot=ipproto_ip) : BaseSocket(t, prot)
        { __initBuf(); } 
    ///@brief	calls BaseSocket(sd)
    SockBuf(int sd) : BaseSocket(sd)
        { __initBuf(); }
    
    ///@brief	closes socket descriptor and flushes output buffer
    void close();
    ///@brief	calls close()
    ~SockBuf();

    using BaseSocket::shutdown;

protected:
    virtual int overflow(int c = EOF);
    virtual int underflow();
    virtual int sync();

private:
    char _inBuf[BUFSIZE], _outBuf[BUFSIZE];
    
    void __initBuf();
};    

///@brief inherits from std::iostream, it allows the C++ stream approach with sockets
class SockStream : public std::iostream {
public:
    SockStream() : std::iostream(new SockBuf()) 
        { exceptions(badbit); }
    ///@brief	calls std::iostream(&s)
    SockStream(SockBuf &s) : std::iostream(&s) 
        { exceptions(badbit); }
    ///@brief	calls std::iostream(new SockBuf(s))
    SockStream(const Socket &s) : std::iostream(new SockBuf(s))
        { exceptions(badbit); }
    ///@brief	copy constructor
    SockStream(const SockStream &s) : std::iostream(s.sockbuf())
        { exceptions(badbit); }
    ///@brief	calls std::iostream(new SockBuf(t,prot))
    SockStream(type t, protocol prot=ipproto_ip) : std::iostream(new SockBuf(t,prot))
        { exceptions(badbit); }
 
    ///@brief	returns pointer to internal SockBuf object
    inline SockBuf* sockbuf() const
    { 
        return (SockBuf *)rdbuf();
    }
    ///@brief	operates on internal SockBuf object
    inline SockBuf* operator->() 
    {
        return sockbuf();
    }
};

///@brief SockStream manipulator which writes "\r\n"
std::ostream &crlf (std::ostream &s)
{
    return s << "\r\n";
}

};

#endif
