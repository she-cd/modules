/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _PORTING_LWIPOPTS_H_
#define _PORTING_LWIPOPTS_H_

#include_next "lwip/lwipopts.h"

#define DHCP_CLIENT_PORT  68
#define DHCP_SERVER_PORT  67

#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_CHECKSUM_ON_COPY           0
#define CHECKSUM_GEN_UDP                1
#define LWIP_SOCKET_SELECT_FUNC         1
#define LWIP_SOCKET_FCNTL_FUNC          0
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_DHCP                       1
#define DHCP_DOES_ARP_CHECK             0
#define ETHARP_TABLE_MATCH_NETIF        0
#endif /* _PORTING_LWIPOPTS_H_ */
