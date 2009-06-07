#include <socket++.h>
#include <algorithm>
#include <sys/time.h>
using namespace std;
using namespace socketpp;

static const double timeo = 0.1;  // seconds
static const double delay = 0.001; // seconds

#define TCP_OPT  0x020405b4

int main(int argc, char **argv)
{
    AddrHandler h;
    TCP_IP_RawSocket sock;
    vector<port_t> ports, closed, filtered;
    string hostname, host_ip;
    int nclosed=0, nfiltered=0;

    if(argc < 2) {
        cerr << argv[0]<< " <target>" <<endl;
        return -1;
    }
    // Custom port array initialization
    for(int i=1; i<=1024; i++) {
        ports.push_back(i);
    }
    srandom(time(NULL));
    random_shuffle(ports.begin(), ports.end());
    
    if(h.isIPv4(argv[1])) {
        host_ip = argv[1];
        hostname = host_ip;
    } else {
        hostname = argv[1];
        host_ip = h.gethostbyname(argv[1])[0];
    }
    sock.connect(host_ip);

    cout <<"Port-scanning "<<hostname <<" ("<< host_ip <<")" <<endl<<endl;

    while(! ports.empty()) {
        double elapsed;
        port_t srcport = random()%16384 + 49152;
        port_t dstport = ports.back();
        ports.pop_back();

        sock.build_IP_header(h.getAddrByRoute(h.inet_aton(host_ip)), h.inet_aton(host_ip));
        sock.build_TCP_header(srcport, dstport, 1, 0, 1024*(random()%4+1), 0, 1);

        long opt = htonl(TCP_OPT);
        sock.set_TCP_options((char *)&opt, sizeof(long));

        sock.adjust_TCP_IP_all();
        sock.settimeout(0);
        sock.send_packet();

        try {
            struct timeval t1, t2;

            elapsed = 0;
            gettimeofday(&t1, NULL);

            while(1) {
                struct iphdr ip_h;
                struct tcphdr tcp_h;

                sock.settimeout(timeo - elapsed);
                sock.read_packet(BUFSIZ);
                sock.get_TCP_header(tcp_h);

                gettimeofday(&t2, NULL);
                elapsed = t2.tv_sec- t1.tv_sec + double(t2.tv_usec-t1.tv_usec)/1000000;

                if(tcp_h.source==dstport && tcp_h.dest==srcport) {
                    if(tcp_h.syn && tcp_h.ack) {
                        cout << "Port "<< dstport<< "/tcp is open"<< endl;
 
                        sock.build_TCP_header(srcport, dstport, 2, 0, 0, 0, 0, 1);
                        sock.build_IP_header(h.getAddrByRoute(h.inet_aton(host_ip)), h.inet_aton(host_ip));
                        sock.adjust_TCP_IP_all();

                        sock.settimeout(0);
                        sock.send_packet();

                        break;
                    }
                    else if(tcp_h.ack && tcp_h.rst) {
                        if(nclosed++ <= 10)
                            closed.push_back(dstport);
                        break;
                    }
                }
            }
    
        } catch(timeout) {
            elapsed = timeo;
            if(nfiltered++ <= 10)
                filtered.push_back(dstport);
        }
        if(delay-elapsed > 0) {
            sleep(delay-elapsed);
            usleep(((delay-elapsed)-int(delay-elapsed))*1000000);
        }
    }

    if(nfiltered <= 10)
        for(vector<port_t>::iterator it=filtered.begin(); it!=filtered.end(); it++)
            cout <<"Port "<< *it<<"/tcp is filtered" <<endl;
    else
        cout <<"Totally "<<nfiltered<<" ports filtered" <<endl;

    if(nclosed <= 10)
        for(vector<port_t>::iterator it=closed.begin(); it!=closed.end(); it++)
            cout <<"Port "<< *it<<"/tcp is closed" <<endl;
    else
        cout <<"Totally "<<nclosed<<" ports closed" <<endl;

    sock.close();
    return 0;
}
