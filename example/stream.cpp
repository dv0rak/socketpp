#include <socket++.h>
#include <iostream>
#include <string>

using namespace socketpp;
using namespace std;

int main()
{
    SockStream ss = SockStream(sock_stream);

    ss->connect("www.google.it", "http");
    
    ss<< "GET / HTTP/1.1"<<crlf;
    ss<< "Host: www.google.it"<<crlf;
    ss<< "Connection: close"<<crlf<<crlf<<flush;

    char line[SockBuf::BUFSIZE];
    while(ss.good()) {
        ss.getline(line,sizeof(line));
        cout<< line<<endl;
    }
    ss->close();
    return 0;
}
