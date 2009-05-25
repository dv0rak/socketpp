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

    sock.settimeout(1.0);
    sock.send_packet();

    try { sock.read_packet(BUFSIZ); }
    catch (timeout &e) { return false; }
 
    return true;
}

int main(int argc, char *argv[])
{
    bool ret;

    if(argc != 2) {
        cerr<< argv[0]<< " <host>"<< endl;
        return -1;
    }
    if(ping(argv[1])) cout<< "Host's up"<< endl;
    else cout<< "Host seems down"<< endl;
    return 0;
}
