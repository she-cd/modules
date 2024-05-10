#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "net_common.h"
#include <sys/types.h>
#include "wm_netif2.0.3.h"
#include "wifi_device_config.h"
#include "bmp5_defs.h"


#define     DEMO_SOCK_BUF_SIZE                512

int bmp5_sockfd = -1;
int bmp5_socket_client_connect();
struct sockaddr_in toAddr;

static void bmp5_udp_c_con_net_status_changed_event(u8 status )
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
            close(bmp5_sockfd);
            bmp5_sockfd = -1;
            break;
        default:
            break;
    }
}


static int bmp5_udp_connect_wifi(char *ssid, char *pwd)
{
    if (!ssid)
    {
        return WM_FAILED;
    }
    printf("ssid:%s\n", ssid);
    printf("password=%s\n",pwd);

    tls_wifi_disconnect();
    tls_netif_add_status_event(bmp5_udp_c_con_net_status_changed_event);

    tls_wifi_connect((u8 *)ssid, strlen(ssid), (u8 *)pwd, strlen(pwd));
    printf("\nplease wait connect net......\n");
    return 0;
}

void judge_sockfd()
{
    tls_os_time_delay(2);
    if(bmp5_sockfd != -1)
    {
        printf("shutting down socket and restaring...\n");
        close(bmp5_sockfd);
        bmp5_sockfd = -1;
    }
}

static int bmp5_udp_client_send(struct bmp5_sensor_data *sensor_data)
{
    int ret;
    char sock_tx1[DEMO_SOCK_BUF_SIZE] = {0};

    sprintf(sock_tx1, "{PA:%.2f,TE:%.2f}", sensor_data->pressure, sensor_data->temperature);
    tls_os_time_delay(500);
    ret = sendto(bmp5_sockfd, sock_tx1, strlen(sock_tx1), 0, (struct sockaddr *)&toAddr, sizeof(toAddr));
    if(ret < 0)
    {
        printf("Error occured during sending, errno:%d\n",errno);
        return -1;
    }
}

int bmp5_udp_client_connect()
{
    int port = 8089;
    u8 ip[20];
    strcpy(ip, "172.20.10.5");

    printf("I will connect to %s:%d\r\n", ip, port);
    bmp5_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    while(1) {
        memset(&toAddr, 0, sizeof(struct sockaddr));
        toAddr.sin_family=AF_INET;
        toAddr.sin_port=htons(port);
        if(inet_pton(AF_INET, ip, &toAddr.sin_addr) <= 0) // UDP socket < 0)
        {
            printf("inet_pton failed!\r\n");
            printf("do_cleanup...\r\n");
            close(bmp5_sockfd);
            return -1;
        } else {
            //printf("inet_pton success!\r\n");
            return 0;
        }
    }
}

int32_t bmp5_udp_client_start(struct bmp5_sensor_data *sensor_data)
{
    if(bmp5_udp_client_send(sensor_data) == -1) {
        printf("%s: send error!\n", __func__);
        return -1;
    }

    return 1;
}

void bmp5_udp_client_init()
{
    struct tls_ethif * ethif;
    u8 ssid[20];
    u8 pwd[20];

    /* AP config */
    //strcpy(ssid, "HUAWEI-B911XA");
    //strcpy(pwd, "12345678");

    strcpy(ssid, "TP-Link_739C");
    strcpy(pwd, "12345678");

    bmp5_connect_wifi(ssid,pwd);
    while(1)
    {
        tls_os_time_delay(1);
        ethif = tls_netif_get_ethif();
        if(ethif->status)
            break;
    }
    //bmp5_udp_client_connect();
}


