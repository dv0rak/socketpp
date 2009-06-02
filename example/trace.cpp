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

int main(int argc, char **argv)
{
    AddrHandler h;
    std::string hostname, host_ip;
    double timeout = 5.0;
    unsigned int nprobes = 3;
    bool ended = false;
 
    if(argc < 3) {
        cerr << argv[0] << " <host> <interface> <timeout=5.0> <nprobes=3>" <<endl;
        return -1;
    }
    if(argc >= 4) {
        stringstream t(argv[3]);
        t >> timeout;
        if(t.fail()) {
            cerr << "`" <<argv[3]<<"` not a valid timeout value" <<endl;
            return -1;
        }
        if(argc >= 5) {
            stringstream n(argv[4]);
            n >> nprobes;
            if(n.fail()) {
                cerr << "`" <<argv[4]<<"` not a valid number of probes" <<endl;
                return -1;
            }
        }
    }
 
    if(h.isIPv4(argv[1])) {
        host_ip = argv[1];
        hostname = host_ip;
    } else {
        hostname = argv[1];
        host_ip = h.gethostbyname(argv[1]);
    }
    h.cleanDnsCache();

    cout << "Tracerouting " << hostname << " (" << host_ip << ")" <<endl<<endl;

    UDP_IP_RawSocket udp; 
    ICMP_RawSocket icmp;

    srandom(time(NULL));

    for (int i=1; !ended; i++) {
        std::string prev_sender_ip = "";

        cout << i<< flush;
 
        for(int j=0; j<nprobes; j++) {
            struct timeval t1;

            udp.build_IP_header(0, 4, 0,0,0,0, i, 0,0, h.getAddrByIface(argv[2]), h.inet_aton(host_ip));
            udp.build_UDP_header(random()%(65535-49152)+49152, start_port+i+j-1, 0,0);
            udp.build_data_payload(payload);
            udp.adjust_UDP_IP_all();
 
            udp.send_packet(host_ip);
            icmp.settimeout(timeout);
            gettimeofday (&t1, NULL);

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
                    icmp.settimeout(double(timeout - delta/1000));

                    if(icmp_h.type==ICMP_TIME_EXCEEDED || (icmp_h.type==ICMP_DEST_UNREACH && host_ip==sender_ip)) {

                        if(sender_ip != prev_sender_ip) { 
                            try { sender_name = h.gethostbyaddr(sender_ip); }
                            catch(h_error) { sender_name = sender_ip; }
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
            } catch(socketpp::timeout) {
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
