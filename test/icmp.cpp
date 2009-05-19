#include <iostream>
#include <socket++.h>
using namespace std;
using namespace socketpp;

int main()
{
    AddrHandler addr;
    ICMP_RawSocket icmp;
        
    icmp.build_ICMP_header(ICMP_ECHO, 1 ,0 ,1,0);
    icmp.build_data_payload("1");
    icmp.adjust_ICMP_csum();

    icmp.connect("www.google.it");
    icmp.send_packet();

    icmp.read_packet(BUFSIZ);

    cout <<"ICMP HEADER :"<<endl;
    socketpp::icmphdr h;
    icmp.get_ICMP_header(h);
    cout<<int(h.type)<<" "<<int(h.code)<<" "<<int(h.check)<<" "<<int(h.id)<<" "<<int(h.sequence)<<endl;

    cout <<"DATA PAYLOAD :"<<endl;
    std::string msg;
    icmp.get_data_payload(msg);
    cout<<msg <<endl;
 
    return 0;
}
