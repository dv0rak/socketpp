/**
 *  Traceroute program partially ripped from Blacklight 
 *  When I'll have some time I'll rewrite it with UDP packets
 */

#include <socket++.h>
#include <sys/time.h>
using namespace std;
using namespace socketpp;

int main(int argc, char **argv)
{
    AddrHandler h;
    in_addr_t addr;
    std::string hostname;
    std::string host_ip;

    if(argc < 3) {
        cerr << argv[0] << " <host> <interface>" <<endl;
        return -1;
    }

    if(h.isIPv4(argv[1])) {
        host_ip = argv[1];
        addr = h.inet_aton(argv[1]);
        try { hostname = h.gethostbyaddr(host_ip); }
        catch (h_error) { hostname = ""; }
        
    } else {
        hostname = argv[1];
        host_ip = h.gethostbyname(argv[1]);
        addr = h.inet_aton(host_ip);
    }

    std::string payload;

    for(int i=0; i<56; i++)
        payload += i;

    bool ended = false;
    struct timeval t1, t2;
    cout << "Tracerouting " << host_ip << " (" << hostname << ")" <<endl<<endl;

    ICMP_IP_RawSocket s;

    for (int i=1; !ended; i++) {
        s.build_IP_header(0, 4, 0,0,0,0, i, 0,0, h.getAddrByIface(argv[2]), addr); 
        s.build_ICMP_header(ICMP_ECHO, 0,0,1,1);
        s.build_data_payload(payload);
        s.adjust_ICMP_IP_all();

        s.send_packet(addr);

        gettimeofday (&t1, NULL);
        bool recvd = false;

        while (!recvd)  {
            std::string sender;
            std::string sender_name;
            socketpp::icmphdr icmp;

            s.read_packet(BUFSIZ, sender);
            s.get_ICMP_header(icmp);

            if(icmp.type==ICMP_TIME_EXCEEDED || (icmp.type==ICMP_ECHOREPLY && host_ip==sender)) {
                gettimeofday (&t2, NULL);
                time_t interval = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_usec-t1.tv_usec)/1000;

                try { sender_name = h.gethostbyaddr(sender); }
                catch(h_error) { sender_name = ""; }

                cout << i << ":\t" << sender << " (" << sender_name << ")"
                     << " - reached in " << interval << "ms\n";

                recvd = true;
                
                if(icmp.type != ICMP_TIME_EXCEEDED)
                    ended = true;
            }
        }
    }
    s.close();
    return 0;
}
