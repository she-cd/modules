#include <stdio.h>
#include <string.h>
#include "wm_include.h"
#include "wm_demo.h"
#include "lwip/errno.h"
#include "wm_netif2.0.3.h"


#define     DEMO_SOCK_BUF_SIZE                512
static const char *sock_tx = "message from client";

int socket_num = -1;
static int socket_client_connect(int port, char *ip);

static void c_con_net_status_changed_event(u8 status )
{
    switch(status)
    {
        case NETIF_IP_NET_UP:
        {
            struct tls_ethif * tmpethif = tls_netif_get_ethif();
#if TLS_CONFIG_IPV4
            print_ipaddr(&tmpethif->ip_addr);
#if TLS_CONFIG_IPV6
            print_ipaddr(&tmpethif->ip6_addr[0]);
            print_ipaddr(&tmpethif->ip6_addr[1]);
            print_ipaddr(&tmpethif->ip6_addr[2]);
#endif
#else
            printf("net up ==> ip = %d.%d.%d.%d\n",ip4_addr1(&tmpethif->ip_addr.addr),ip4_addr2(&tmpethif->ip_addr.addr),
                             ip4_addr3(&tmpethif->ip_addr.addr),ip4_addr4(&tmpethif->ip_addr.addr));
                    
#endif
        }
            break;
        
        case NETIF_WIFI_JOIN_FAILED:
            printf("NETIF_WIFI_JOIN_FAILED\n");
            break;
        case NETIF_WIFI_JOIN_SUCCESS:
            printf("NETIF_WIFI_JOIN_SUCCESS\n");
            break;
        case NETIF_WIFI_DISCONNECTED:
            printf("NETIF_WIFI_DISCONNECTED\n");
            //shutdown(socket_num,0);
            closesocket(socket_num);
            socket_num = -1;
            break;
        default:
            break;
    }
}


static int c_connect_wifi(char *ssid, char *pwd)
{
    if (!ssid)
    {
        return WM_FAILED;
    }
    printf("ssid:%s\n", ssid);
    printf("password=%s\n",pwd);

    tls_wifi_disconnect();
    tls_netif_add_status_event(c_con_net_status_changed_event);

    tls_wifi_connect((u8 *)ssid, strlen(ssid), (u8 *)pwd, strlen(pwd));
    printf("\nplease wait connect net......\n");
    return 0;
}


static int socket_client_connect(int port, char *ip)
{
    int ret;
    char sock_rx[DEMO_SOCK_BUF_SIZE] = {0};
    char sock_tx1[DEMO_SOCK_BUF_SIZE] = {0};
    int send_data = 0;
    struct sockaddr_in pin;
    printf("\nport:%d,ip:%s\n",port,ip);

    while(1)
    {
        memset(&pin, 0, sizeof(struct sockaddr));
        pin.sin_family=AF_INET;

        pin.sin_addr.s_addr = ipaddr_addr(ip);
        pin.sin_port=htons(port);
        if((socket_num = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
        {
            printf("creat socket fail, errno:%d\n",errno);
            return -1;
        }
        
        if (connect(socket_num, (struct sockaddr *)&pin, sizeof(struct sockaddr)) != 0)
        {
            printf("connect fail errno:%d\n",errno);
        }
        else
        {
            printf("connect success\n");
        }
        while(1)
        {
            if(send_data < 40) {
                sprintf(sock_tx1, "{PA:94770.42,TE:%f}", 27.21);
                send_data++;
            }
            tls_os_time_delay(1000);
              ret = send(socket_num, sock_tx1, strlen(sock_tx1), 0);
            if(ret < 0)
            {
                printf("Error occured during sending, errno:%d\n",errno);
                break;
            }

            tls_os_time_delay(2);
        }
        if(socket_num != -1)
        {
            printf("shutting down socket and restaring...\n");
            shutdown(socket_num,0);
            closesocket(socket_num);
            socket_num = -1;
        }
    }
}    

int demo_socket_client(char *ssid, char *pwd,int port,char *ip)
{
    struct tls_ethif * ethif;
    c_connect_wifi(ssid,pwd);
    while(1)
    {
        tls_os_time_delay(1);
        ethif = tls_netif_get_ethif();
        if(ethif->status)
            break;
    }
    socket_client_connect(port,ip);
    return 0;
}

void Start_socket_client()
{
    printf("%s: %d\n", __func__, __LINE__);

    u8 ssid[20];
    u8 pwd[20];
    int port;
    u8 ip[20];

    /* AP config */
    strcpy(ssid, "HUAWEI-B911XA");
    strcpy(pwd, "liyong3838");
    strcpy(ip, "192.168.3.108");
    demo_socket_client(ssid, pwd, 8089, ip);
}


