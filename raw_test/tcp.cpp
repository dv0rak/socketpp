#include <iostream>
#include <socket++.h>
using namespace std;
using namespace socketpp;

#define IFACE "eth1"

int main()
{
    AddrHandler addr;

    TCP_RawSocket tcp;
    tcp.connect("www.google.it");

    tcphdr tcp_h;
    memset(&tcp_h, 0, sizeof tcp_h);
    tcp_h.source = 12345;
    tcp_h.dest   = 80;
    tcp_h.seq    = 1;
    tcp_h.window = 1024;
    tcp_h.syn    = 1; 
    tcp.build_TCP_header(tcp_h);

    tcp.adjust_TCP_all(addr.getAddrByIface(IFACE),addr.inet_aton(addr.gethostbyname("www.google.it")));

    tcp.send_packet();

    tcp.read_packet(BUFSIZ);

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
