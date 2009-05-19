#include <iostream>
#include <socket++.h>
using namespace std;
using namespace socketpp;

#define IFACE "eth1"

int main()
{
    AddrHandler addr;

    TCP_IP_RawSocket tcp;
    tcp.connect("www.google.it");

    tcphdr tcp_h;
    memset(&tcp_h, 0, sizeof tcp_h);
    tcp_h.source = 12345;
    tcp_h.dest   = 80;
    tcp_h.seq    = 1;
    tcp_h.window = 1024;
    tcp_h.syn    = 1; 
    tcp.build_TCP_header(tcp_h);

    iphdr ip_h;
    ip_h.protocol   = ipproto_tcp;
    ip_h.version 	= 4;
    ip_h.frag_off   = 0;
    ip_h.ttl	= 64;
    ip_h.daddr 	= addr.inet_aton(addr.gethostbyname("www.google.it"));
    ip_h.saddr	= get_iface_ip(IFACE);
    tcp.build_IP_header(ip_h);
 
    tcp.adjust_TCP_IP_all();

    tcp.send_packet();

    tcp.read_packet(BUFSIZ);

    cout <<"IP HEADER :"<<endl;
    tcp.get_IP_header(ip_h);
    cout<<(int)ip_h.ihl<<" "<<(int)ip_h.version<<" "<<(int)ip_h.tos<<" "<<(int)ip_h.tot_len<<" "
        <<(int)ip_h.id<<" "<<(int)ip_h.frag_off<<" "<<(int)ip_h.ttl<<" "<<(int)ip_h.protocol<<" "
        <<(int)ip_h.check<<" "<<(int)ip_h.saddr<<" "<<(int)ip_h.daddr<<endl;

    cout <<"IP OPTIONS :"<<endl;
    std::string opt;
    tcp.get_IP_options(opt);
    cout<< opt <<endl;

    cout <<"TCP HEADER :"<<endl;
    tcphdr h;
    tcp.get_TCP_header(h);
    cout <<(int) h.source<<" " <<(int) h.dest<<" " <<(int) h.seq<<" " <<(int) h.ack_seq<<" " <<(int) h.res1<<" " 
         <<(int) h.doff<<" "<<(int) h.fin<<" " <<(int) h.syn<<" " <<(int) h.rst<<" " <<(int) h.psh<<" " 
         <<(int) h.ack<<" " <<(int) h.urg<<" "<<(int) h.ece<<" " <<(int) h.cwr<<" " <<(int) h.window<<" " 
         <<(int) h.check<<" " <<(int) h.urg_ptr<<endl;

    cout << "SA: " << ((h.syn&h.ack)? "yes" : "no") << endl;

    cout<< "TCP OPTIONS :"<<endl;
    std::string buf;
    tcp.get_TCP_options(buf);
    cout<<buf<<endl;
 
    cout <<"DATA PAYLOAD :"<<endl;
    std::string msg;
    tcp.get_data_payload(msg);
    cout<<msg <<endl;
 
    return 0;
}
