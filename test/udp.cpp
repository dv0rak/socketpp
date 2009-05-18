#include <iostream>
#include <socket++.h>
using namespace std;
using namespace socketpp;

int main()
{
    try {
        AddrHandler addr;

        UDP_RawSocket udp;
        udp.connect("localhost");

        udp.build_UDP_header(6000, 13 ,0 ,0);
        udp.build_data_payload("1");

        udp.adjust_UDP_all(inaddr_loopback, inaddr_loopback);

        udp.send_packet();

        udp.read_packet(BUFSIZ);
        udp.read_packet(BUFSIZ);

        cout <<"UDP HEADER :"<<endl;
        udphdr h;
        udp.get_UDP_header(h);
        cout<<int(h.source)<<" "<<int(h.dest)<<" "<<int(h.check)<<" "<<int(h.len)<<endl;

        cout <<"DATA PAYLOAD :"<<endl;
        std::string msg;
        udp.get_data_payload(msg);
        cout<<msg <<endl;
 
    } catch (SockException &e) {
        cerr <<e.what() <<endl;
    }
    return 0;
}
