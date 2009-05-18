#include <socket++.h>
#include <iostream>
using namespace std;
using namespace socketpp;

bool ping(const string& addr)
{
    ICMP_RawSocket sock;
    sock.connect(addr);

    sock.build_ICMP_header(ICMP_ECHO, 0, 0, 1, 1);
    sock.adjust_ICMP_csum();

    sock.send_packet();

    sock.settimeout(1);

    try { sock.read_packet(BUFSIZ); }
    catch (SockException &e) { return false; }
 
    return true;
}

int main(int argc, char *argv[])
{
    bool ret;

    if(argc != 2) {
        cerr<< argv[0]<< " <host>"<< endl;
        return -1;
    }
    if(geteuid()) {
        cerr<< "Must be root"<< endl;
        return -1;
    }

    try {
        if(ping(argv[1])) cout<< "Host's up"<< endl;
        else cout<< "Host seems down"<< endl;
    } catch(SockException &e) {
        cerr<< e.what() <<endl;
        return -1;
    }
    return 0;
}
