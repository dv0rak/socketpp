#include <iostream>
#include <socket++.h>
using namespace std;
using namespace socketpp;

#define IFACE "eth1"

int main()
{
    try {
        AddrHandler addr;

        ICMP_IP_RawSocket icmp;
        icmp.connect("www.google.it");

        icmp.build_ICMP_header(ICMP_ECHO, 1 ,0 ,1,0);
        icmp.build_data_payload("1");

        iphdr ip_h;
        ip_h.protocol   = ipproto_icmp;
        ip_h.version 	= 4;
        ip_h.frag_off   = 0;
        ip_h.ttl	= 64;
        ip_h.saddr 	= get_iface_ip(IFACE);
        ip_h.daddr	= addr.inet_aton(addr.gethostbyname("www.google.it"));
        icmp.build_IP_header(ip_h);
 
        icmp.adjust_ICMP_IP_all();

        icmp.send_packet();

        icmp.read_packet(BUFSIZ);

        cout <<"IP HEADER :"<<endl;
        icmp.get_IP_header(ip_h);
        cout<<(int)ip_h.ihl<<" "<<(int)ip_h.version<<" "<<(int)ip_h.tos<<" "<<(int)ip_h.tot_len<<" "
            <<(int)ip_h.id<<" "<<(int)ip_h.frag_off<<" "<<(int)ip_h.ttl<<" "<<(int)ip_h.protocol<<" "
            <<(int)ip_h.check<<" "<<(int)ip_h.saddr<<" "<<(int)ip_h.daddr<<endl;

        cout <<"IP OPTIONS :"<<endl;
        std::string opt;
        icmp.get_IP_options(opt);
        cout<< opt <<endl;

        cout <<"ICMP HEADER :"<<endl;
        socketpp::icmphdr h;
        icmp.get_ICMP_header(h);
        cout<<int(h.type)<<" "<<int(h.code)<<" "<<int(h.check)<<" "<<int(h.id)<<" "<<int(h.sequence)<<endl;

        cout <<"DATA PAYLOAD :"<<endl;
        std::string msg;
        icmp.get_data_payload(msg);
        cout<<msg <<endl;
 
    } catch (SockException &e) {
        cerr <<e.what() <<endl;
    }
    return 0;
}
