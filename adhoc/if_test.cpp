#include<iostream>
#include<sys/types.h>
#include<ifaddrs.h>
#include<string>
#include<netinet/in.h>
#include <arpa/inet.h>
#include<array>
using namespace std;

 struct if_info{
        string if_name;
        array<uint8_t,4> ip_addr;
        array<uint8_t,4> brd_addr;
    };
int main(void){
    struct ifaddrs * addrs = nullptr;
    int ret = getifaddrs(&addrs);
    if(ret != 0) {
        fprintf(stderr, "!!! getifaddrs [%s:L%d]\n", __FILE__, __LINE__);
        return 1;
    }
    for(auto p = addrs; p!=nullptr; p=p->ifa_next){
        string interface_name = string(p->ifa_name);
        string disp_ipaddress;
        string disp_netmask;
        sa_family_t address_family = p->ifa_addr->sa_family;
        printf("sa:%x,%x\n",address_family,p->ifa_addr->sa_family);
        if(address_family==AF_INET){ // IPv4
            char buffer[INET_ADDRSTRLEN]={0,0,0,0};
            if(p->ifa_addr != nullptr){
                inet_ntop(address_family, &((struct sockaddr_in*)p->ifa_addr)->sin_addr,
                    buffer, INET_ADDRSTRLEN);
                disp_ipaddress = string(buffer);
            }
            if(p->ifa_netmask!=nullptr){
                char buffer[INET_ADDRSTRLEN]={0,0,0,0};
                inet_ntop(address_family, &((struct sockaddr_in*)(p->ifa_netmask))->sin_addr,
                    buffer, INET_ADDRSTRLEN);
         
                disp_netmask = string(buffer);
            }
            printf("IF:%s IPv4: %s  mask:%s\n",
                interface_name.c_str(), disp_ipaddress.c_str(), disp_netmask.c_str());
        }/*
        else if(address_family == AF_INET6){ // IPv6
            uint32_t scope_id=0;
            if(p->ifa_addr!=nullptr){
                char buffer[INET6_ADDRSTRLEN] = {0,0,0,0};
                inet_ntop(address_family, &((struct sockaddr_in6*)(p->ifa_addr))->sin6_addr,
                buffer, INET6_ADDRSTRLEN);
                disp_ipaddress = string(buffer);
                scope_id = ((struct sockaddr_in6*)(p->ifa_addr))->sin6_scope_id;
            }
            if(p->ifa_netmask!=nullptr){
                char buffer[INET6_ADDRSTRLEN] = {0,};
                inet_ntop(address_family, &((struct sockaddr_in6*)(p->ifa_netmask))->sin6_addr,
                buffer, INET6_ADDRSTRLEN);
                disp_netmask = string(buffer);
            }
            printf("IF:%s IPv6: %s  mask:%s scope id:%d\n",
                interface_name.c_str(), disp_ipaddress.c_str(), disp_netmask.c_str(), scope_id);
        }*/
    }
    freeifaddrs(addrs);
    return 0;
}