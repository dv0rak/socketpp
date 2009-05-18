#include <iostream>
#include <socket++.h>
using namespace std;
using namespace socketpp;

int main()
{
    try {
        AddrHandler addr;

        UDP_IP_RawSocket udp;
        udp.connect("localhost");

        udp.build_UDP_header(6000, 13 ,0 ,0);
        udp.build_data_payload("1");

        iphdr ip_h;
        ip_h.protocol   = ipproto_udp;
        ip_h.version 	= 4;
        ip_h.frag_off   = 0;
        ip_h.ttl	= 64;
        ip_h.saddr 	= inaddr_loopback;
        ip_h.daddr	= inaddr_loopback;
        udp.build_IP_header(ip_h);
 
        udp.adjust_UDP_IP_all();

        udp.send_packet();

        udp.read_packet(BUFSIZ);
        udp.read_packet(BUFSIZ);

        cout <<"IP HEADER :"<<endl;
        udp.get_IP_header(ip_h);
        cout<<(int)ip_h.ihl<<" "<<(int)ip_h.version<<" "<<(int)ip_h.tos<<" "<<(int)ip_h.tot_len<<" "
            <<(int)ip_h.id<<" "<<(int)ip_h.frag_off<<" "<<(int)ip_h.ttl<<" "<<(int)ip_h.protocol<<" "
            <<(int)ip_h.check<<" "<<(int)ip_h.saddr<<" "<<(int)ip_h.daddr<<endl;

        cout <<"IP OPTIONS :"<<endl;
        std::string opt;
        udp.get_IP_options(opt);
        cout<< opt <<endl;

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
