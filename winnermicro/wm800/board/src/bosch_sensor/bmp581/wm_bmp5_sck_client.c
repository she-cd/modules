#include <stdio.h>
#include <string.h>
#include "wm_include.h"
#include "wm_demo.h"
#include "lwip/errno.h"
#include "wm_netif2.0.3.h"
#include "bmp5_defs.h"


#define     DEMO_SOCK_BUF_SIZE                512

int bmp5_socket_num = -1;
int bmp5_socket_client_connect();

static void bmp5_c_con_net_status_changed_event(u8 status )
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
            //shutdown(bmp5_socket_num,0);
            closesocket(bmp5_socket_num);
            bmp5_socket_num = -1;
            break;
        default:
            break;
    }
}


int bmp5_connect_wifi(char *ssid, char *pwd)
{
    if (!ssid)
    {
        return WM_FAILED;
    }
    printf("ssid:%s\n", ssid);
    printf("password=%s\n",pwd);

    tls_wifi_disconnect();
    tls_netif_add_status_event(bmp5_c_con_net_status_changed_event);

    tls_wifi_connect((u8 *)ssid, strlen(ssid), (u8 *)pwd, strlen(pwd));
    printf("\nplease wait connect net......\n");
    return 0;
}

void judge_socket_num()
{
    tls_os_time_delay(2);
    if(bmp5_socket_num != -1)
    {
        printf("shutting down socket and restaring...\n");
        shutdown(bmp5_socket_num,0);
        closesocket(bmp5_socket_num);
        bmp5_socket_num = -1;
    }
}

static int bmp5_socket_client_send(struct bmp5_sensor_data *sensor_data)
{
    int ret;
    char sock_rx[DEMO_SOCK_BUF_SIZE] = {0};
    char sock_tx1[DEMO_SOCK_BUF_SIZE] = {0};

    sprintf(sock_tx1, "{PA:%.2f,TE:%.2f}", sensor_data->pressure, sensor_data->temperature);
    tls_os_time_delay(1000);
    ret = send(bmp5_socket_num, sock_tx1, strlen(sock_tx1), 0);
    if(ret < 0)
    {
        printf("Error occured during sending, errno:%d\n",errno);
        return -1;
    }
}

int bmp5_socket_client_connect()
{
    struct sockaddr_in pin;
    int port = 8089;
    u8 ip[20];
    strcpy(ip, "192.168.3.114");

    while(1) {
        memset(&pin, 0, sizeof(struct sockaddr));
        pin.sin_family=AF_INET;

        pin.sin_addr.s_addr = ipaddr_addr(ip);
        pin.sin_port=htons(port);
        if((bmp5_socket_num = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
        {
            printf("creat socket fail, errno:%d\n",errno);
            return -1;
        }
        tls_os_time_delay(1000);
        if (connect(bmp5_socket_num, (struct sockaddr *)&pin, sizeof(struct sockaddr)) != 0)
        {
            printf("connect fail errno:%d\n",errno);
        }
        else
        {
            printf("connect success\n");
            return 0;
        }
    }
}

void bmp5_socket_client_start(struct bmp5_sensor_data *sensor_data)
{
    bmp5_socket_client_send(sensor_data);
}

void bmp5_socket_client_init()
{
    struct tls_ethif * ethif;
    u8 ssid[20];
    u8 pwd[20];

    /* AP config */
    //strcpy(ssid, "HUAWEI-B911XA");
    //strcpy(pwd, "liyong3838");

    strcpy(ssid, "TP-lINK_739C");
    strcpy(pwd, "12345678");

    bmp5_connect_wifi(ssid,pwd);
    while(1)
    {
        tls_os_time_delay(1);
        ethif = tls_netif_get_ethif();
        if(ethif->status)
            break;
    }
    bmp5_socket_client_connect();
}


