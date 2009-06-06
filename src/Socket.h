#ifndef __SOCKET_H
#define __SOCKET_H TRUE

#include "BaseSocket.h"

namespace socketpp {

///@brief	inherits from BaseSocket, it aims to be a simple C-socket wrapper
class Socket : public BaseSocket {
public:
    Socket() : BaseSocket() { }
    ///@brief	copy constructor
    Socket(const Socket &s) : BaseSocket(s) { }
    ///@brief	calls BaseSocket(t,prot)
    Socket(type t, protocol prot=ipproto_ip) : BaseSocket(t, prot) { }
    ///@brief	calls BaseSocket(sd)
    Socket(int sd) : BaseSocket(sd) { }

    using BaseSocket::send;
    using BaseSocket::recv;
    using BaseSocket::sendto;
    using BaseSocket::recvfrom;
    using BaseSocket::shutdown;
};

};
#endif
