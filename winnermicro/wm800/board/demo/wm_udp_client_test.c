/*
 * Copyright (C) 2022 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

//#include "net_demo.h"
#include "net_common.h"
#include <sys/types.h>
#include "wm_netif2.0.3.h"
#include "wifi_device_config.h"

int sockfd = -1;
static char request[] = "Hello.";
static char response[128] = "";

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
            close(sockfd);
            sockfd = -1;
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

void UdpClientTest(const char* host, unsigned short port)
{
    ssize_t retval = 0;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket

    struct sockaddr_in toAddr = {0};

while(1) {
    memset(&toAddr, 0, sizeof(struct sockaddr));
    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(port); // 端口号，从主机字节序转为网络字节序
    if (inet_pton(AF_INET, host, &toAddr.sin_addr) <= 0) { // 将主机IP地址从“点分十进制”字符串 转化为 标准格式（32位整数）
        printf("inet_pton failed!\r\n");
        printf("do_cleanup...\r\n");
        close(sockfd);
    }

    // UDP socket 是 “无连接的” ，因此每次发送都必须先指定目标主机和端口，主机可以是多播地址
    tls_os_time_delay(1000);
    retval = sendto(sockfd, request, sizeof(request), 0, (struct sockaddr *)&toAddr, sizeof(toAddr));
    if (retval < 0) {
        printf("sendto failed!\r\n");
        printf("do_cleanup...\r\n");
        close(sockfd);
    }
    printf("send UDP message {%s} %ld done!\r\n", request, retval);
    }
}

void NetDemoTest(unsigned short port, const char* host) {
    (void) host;
    printf("I will connect to %s:%d\r\n", host, port);
    UdpClientTest(host, port);
}

static void demo_udp_client(char *ssid, char *pwd,int port,char *ip)
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
    NetDemoTest(port,ip);
    return 0;
}

void Start_udp_client()
{
    printf("%s: %d\n", __func__, __LINE__);

    u8 ssid[20];
    u8 pwd[20];
    int port;
    u8 ip[20];

    /* AP config */
    strcpy(ssid, "HUAWEI-B911XA");
    strcpy(pwd, "liyong3838");
    strcpy(ip, "192.168.3.114");
    demo_udp_client(ssid, pwd, 8089, ip);
}