/**
 * @file
 * Common IPv4 and IPv6 code
 *
 * @defgroup ip IP
 * @ingroup callbackstyle_api
 * 
 * @defgroup ip4 IPv4
 * @ingroup ip
 *
 * @defgroup ip6 IPv6
 * @ingroup ip
 * 
 * @defgroup ipaddr IP address handling
 * @ingroup infrastructure
 * 
 * @defgroup ip4addr IPv4 only
 * @ingroup ipaddr
 * 
 * @defgroup ip6addr IPv6 only
 * @ingroup ipaddr
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#if LWIP_IPV4 || LWIP_IPV6

#include "lwip/ip_addr.h"
#include "lwip/ip.h"

void print_ipaddr(ip_addr_t *ip)
{
#if LWIP_IPV6
        if(IP_IS_V6(ip))
        {
            printf("ip: %x:%x:%x:%x:%x:%x:%x:%x\n", IP6_ADDR_BLOCK1(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK2(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK3(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK4(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK5(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK6(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK7(ip_2_ip6(ip)), \
                                                        IP6_ADDR_BLOCK8(ip_2_ip6(ip)));                                                        
        }
        else
#endif        
        {        
 #if LWIP_IPV4
            printf("ip: %d.%d.%d.%d\n", ip4_addr1(ip_2_ip4(ip)), \
                                                        ip4_addr2(ip_2_ip4(ip)), \
                                                        ip4_addr3(ip_2_ip4(ip)), \
                                                        ip4_addr4(ip_2_ip4(ip)));
#endif                                                        
        }
}


#endif /* LWIP_IPV4 || LWIP_IPV6 */
