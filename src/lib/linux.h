#include <stdio.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <regex>

#include <sys/time.h>

long long currentTimestampMs() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

// https://stackoverflow.com/questions/4656824/get-lan-ip-and-print-it
int getLanAddress(std::string& retAddress) {
    struct ifaddrs *iflist, *iface;

    if (getifaddrs(&iflist) < 0) {
        perror("getifaddrs");
        return -1;
    }

    std::regex e("^192\\.168\\.[0-9]{1,3}\\.[0-9]{1,3}$");

    for (iface = iflist; iface; iface = iface->ifa_next) {
        int af = iface->ifa_addr->sa_family;
        const void *addr;
        char addrp[INET6_ADDRSTRLEN];

        switch (af) {
            case AF_INET:
                addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
                break;
            case AF_INET6:
                addr = &((struct sockaddr_in6 *)iface->ifa_addr)->sin6_addr;
                break;
            default:
                addr = NULL;
        }

        if (addr) {
            if (inet_ntop(af, addr, addrp, sizeof addrp) == NULL) {
                perror("inet_ntop");
                continue;
            }

            std::string s = addrp;
            if (std::regex_match(s,e)) {
              retAddress.assign(s);
              freeifaddrs(iflist);
              return 1;
            }

            //printf("Interface %s has address %s\n", iface->ifa_name, addrp);
        }
    }

    freeifaddrs(iflist);
    return 0;
}
