// Traceroute program partially ripped from Blacklight
 
#include <socket++.h>
#include <sys/time.h>
#include <cstdlib>
#include <sstream>
using namespace std;
using namespace socketpp;

const char * payload = "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f" \
                       "\x50\x51\x52\x53\x55\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f";
const int start_port = 33434;
static const double timeo = 5.0;
static const unsigned int nprobes = 3;

int main(int argc, char **argv)
{
    AddrHandler h;
    std::string host_ip;
    bool ended = false;
 
    if(argc < 2) {
        cerr << argv[0] << " <host>" <<endl;
        return -1;
    }

    host_ip = h.gethostbyname(argv[1])[0];

    cout << "Tracerouting " << argv[1] << " (" << host_ip << ")" <<endl<<endl;

    Socket udp(sock_dgram); 
    ICMP_RawSocket icmp;

    srandom(time(NULL));

    for (int ttl=1; !ended; ttl++) {
        std::string prev_sender_ip = "";

        cout << ttl<< flush;
 
        for(int i=0; i<nprobes; i++) {
            struct timeval t1;

            udp.setsockopt(sol_ip, ip_ttl, ttl);
            udp.sendto(string(payload), host_ip, start_port+(ttl-1)*nprobes+i);
            gettimeofday (&t1, NULL);

            icmp.settimeout(timeo);

            try {
                while(1) {
                    double delta;
                    struct timeval t2;
                    std::string sender_ip, sender_name;
                    socketpp::icmphdr icmp_h;

                    icmp.read_packet(BUFSIZ, sender_ip);
                    icmp.get_ICMP_header(icmp_h);

                    gettimeofday(&t2, NULL);
                    delta = (t2.tv_sec-t1.tv_sec)*1000 + double(t2.tv_usec-t1.tv_usec)/1000;
                    icmp.settimeout(double(timeo - delta/1000));

                    if(icmp_h.type==ICMP_TIME_EXCEEDED || (icmp_h.type==ICMP_DEST_UNREACH && host_ip==sender_ip)) {

                        if(sender_ip != prev_sender_ip) { 
                            sender_name = h.gethostbyaddr(sender_ip);
                            prev_sender_ip = sender_ip;
                            cout<<" "<<  sender_name <<" (" <<sender_ip <<")  " <<delta <<" ms" <<flush;
                        } else {
                            cout<< "  " <<delta <<" ms" <<flush;
                        }

                        if(icmp_h.type == ICMP_DEST_UNREACH)
                            ended = true;

                        break;
                    }
                }
            } catch(timeout) {
                prev_sender_ip = "";
                cout <<" *" <<flush;
            }
        }
        cout <<endl;
    }
    udp.close();
    icmp.close();
    return 0;
}
