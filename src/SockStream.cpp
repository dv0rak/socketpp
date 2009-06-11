#include "SockStream.h"

namespace socketpp {

void SockBuf::close()
{
    sync();
    BaseSocket::close();
}

int SockBuf::underflow()
{
    int n;
    if((n=recv(_inBuf,BUFSIZE)) <= 0)
        return EOF;
    setg(eback(), _inBuf, _inBuf+n);
    return *gptr();
}

int SockBuf::sync()
{
    return(overflow(EOF)==EOF? -1 : 0);
}

int SockBuf::overflow(int c)
{
    if(send(pbase(),pptr()-pbase()) < 0)
        return EOF;
    setp(pbase(), pbase()+BUFSIZE);
    if(c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    return 0;
}

void SockBuf::_initBuf()
{
    setp(_outBuf, _outBuf+BUFSIZE);
    setg(_inBuf, _inBuf, _inBuf);
}

SockBuf::~SockBuf()
{
    close();
}

};
