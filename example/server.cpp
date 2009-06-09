#include <socket++.h>
#include <iostream>
using namespace std;
using namespace socketpp;

void client(Socket &s,void *)
{
    SockStream ss(s);
    try {
        ss<<"HELLO " <<ss->remoteAddr() <<":" <<ss->remotePort() <<crlf;
        ss->close();
    } catch (error &e) {
        cerr<< e.what()<<endl;
    }
}

int main()
{
    SocketServer serv = SocketServer(sock_stream, inaddr_any, 12345, 5);
    serv.threadClientHandle(&client, NULL);
    return 0;
}
