#ifndef __SOCKET_H
#define __SOCKET_H TRUE

#include "BaseSocket.h"

namespace socketpp {

///@brief	inherits from BaseSocket, it's simply a C-socket wrapper
class Socket : public BaseSocket {
public:
    Socket() : BaseSocket() { }
    ///@brief	calls BaseSocket(t,prot)
    explicit Socket(type t, protocol prot=ipproto_ip) : BaseSocket(t, prot) { }
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
