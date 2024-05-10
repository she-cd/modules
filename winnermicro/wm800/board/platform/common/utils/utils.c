/*
 * Copyright (c) 2022 Winner Microelectronics Co., Ltd. All rights reserved.
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

/**************************************************************************
 * File Name                   : utils.c
 * Author                      :
 * Version                     : 1.0
 * Date                        :
 * Description                 :
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 * All rights reserved.
 *
 ***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "wm_mem.h"
#include "tls_common.h"
#include "wm_debug.h"
#include "utils.h"

static const u8 crc8_tbl[256] = {
    0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4, 0x75,
    0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b,
    0x1c, 0x8d, 0xff, 0x6e, 0x1b, 0x8a, 0xf8, 0x69,
    0x12, 0x83, 0xf1, 0x60, 0x15, 0x84, 0xf6, 0x67,
    0x38, 0xa9, 0xdb, 0x4a, 0x3f, 0xae, 0xdc, 0x4d,
    0x36, 0xa7, 0xd5, 0x44, 0x31, 0xa0, 0xd2, 0x43,
    0x24, 0xb5, 0xc7, 0x56, 0x23, 0xb2, 0xc0, 0x51,
    0x2a, 0xbb, 0xc9, 0x58, 0x2d, 0xbc, 0xce, 0x5f,
    0x70, 0xe1, 0x93, 0x02, 0x77, 0xe6, 0x94, 0x05,
    0x7e, 0xef, 0x9d, 0x0c, 0x79, 0xe8, 0x9a, 0x0b,
    0x6c, 0xfd, 0x8f, 0x1e, 0x6b, 0xfa, 0x88, 0x19,
    0x62, 0xf3, 0x81, 0x10, 0x65, 0xf4, 0x86, 0x17,
    0x48, 0xd9, 0xab, 0x3a, 0x4f, 0xde, 0xac, 0x3d,
    0x46, 0xd7, 0xa5, 0x34, 0x41, 0xd0, 0xa2, 0x33,
    0x54, 0xc5, 0xb7, 0x26, 0x53, 0xc2, 0xb0, 0x21,
    0x5a, 0xcb, 0xb9, 0x28, 0x5d, 0xcc, 0xbe, 0x2f,
    0xe0, 0x71, 0x03, 0x92, 0xe7, 0x76, 0x04, 0x95,
    0xee, 0x7f, 0x0d, 0x9c, 0xe9, 0x78, 0x0a, 0x9b,
    0xfc, 0x6d, 0x1f, 0x8e, 0xfb, 0x6a, 0x18, 0x89,
    0xf2, 0x63, 0x11, 0x80, 0xf5, 0x64, 0x16, 0x87,
    0xd8, 0x49, 0x3b, 0xaa, 0xdf, 0x4e, 0x3c, 0xad,
    0xd6, 0x47, 0x35, 0xa4, 0xd1, 0x40, 0x32, 0xa3,
    0xc4, 0x55, 0x27, 0xb6, 0xc3, 0x52, 0x20, 0xb1,
    0xca, 0x5b, 0x29, 0xb8, 0xcd, 0x5c, 0x2e, 0xbf,
    0x90, 0x01, 0x73, 0xe2, 0x97, 0x06, 0x74, 0xe5,
    0x9e, 0x0f, 0x7d, 0xec, 0x99, 0x08, 0x7a, 0xeb,
    0x8c, 0x1d, 0x6f, 0xfe, 0x8b, 0x1a, 0x68, 0xf9,
    0x82, 0x13, 0x61, 0xf0, 0x85, 0x14, 0x66, 0xf7,
    0xa8, 0x39, 0x4b, 0xda, 0xaf, 0x3e, 0x4c, 0xdd,
    0xa6, 0x37, 0x45, 0xd4, 0xa1, 0x30, 0x42, 0xd3,
    0xb4, 0x25, 0x57, 0xc6, 0xb3, 0x22, 0x50, 0xc1,
    0xba, 0x2b, 0x59, 0xc8, 0xbd, 0x2c, 0x5e, 0xcf
};

#ifndef isdigit
#define in_range(c, lo, up)  ((u8)(c) >= (lo) && (u8)(c) <= (up))
#define isdigit(c)           in_range((c), '0', '9')
#endif
int chk_crc8(u8 *ptr, u32 len)
{
    u8 crc8;
    u8 data;

    crc8 = 0;
    while (len-- != 0) {
        data = *ptr++;
        crc8 = crc8_tbl[crc8 ^ data];
    }

    if (crc8 == 0x00) {
        return 0;
    } else {
        return -1;
    }
}

// #ifndef TLS_CONFIG_FPGA
u8 get_crc8(u8 *ptr, u32 len)
{
    u8 crc8;
    u8 data;

    crc8 = 0;
    while (len-- != 0) {
        data = *ptr++;
        crc8 = crc8_tbl[crc8 ^ data];
    }

    return crc8;
}
// #endif

u8 calculate_crc8(u8 crc8, u8 *ptr, u32 len)
{
    u8 data;

    while (len-- != 0) {
        data = *ptr++;
        crc8 = crc8_tbl[crc8 ^ data];
    }

    return crc8;
}

static u32 _cal_crc32(u32 crc_result, u8 data_8)
{
    u8 crc_out[32];
    u8 crc_buf[32];
    u8 in_data_buf[8];
    u32 i;
    u32 flag;

    flag = 0x01;

    for (i = 0; i < 32; i++) { // 32:loop cap
        crc_out[i] = 0;
    }

    for (i = 0; i < 8; i++) { // 8:loop cap
        in_data_buf[i] = (data_8 >> i) & flag;
    }

    for (i = 0; i < 32; i++) { // 32:loop cap
        crc_buf[i] = (unsigned char)(crc_result >> i) & flag;
    }

    crc_out[0]  = in_data_buf[1]^in_data_buf[7]^crc_buf[30]^crc_buf[24];
    crc_out[1]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[6]^in_data_buf[7]^crc_buf[31]^crc_buf[30]^
                crc_buf[25]^crc_buf[24];
    crc_out[2]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[5]^in_data_buf[6]^in_data_buf[7]^crc_buf[31]^
                crc_buf[30]^crc_buf[26]^crc_buf[25]^crc_buf[24];
    crc_out[3]  = in_data_buf[0]^in_data_buf[4]^in_data_buf[5]^in_data_buf[6]^crc_buf[31]^crc_buf[27]^
                crc_buf[26]^crc_buf[25];
    crc_out[4]  = in_data_buf[1]^in_data_buf[3]^in_data_buf[4]^in_data_buf[5]^in_data_buf[7]^crc_buf[30]^
                crc_buf[28]^crc_buf[27]^crc_buf[26]^crc_buf[24];
    crc_out[5]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[2]^in_data_buf[3]^in_data_buf[4]^in_data_buf[6]^
                in_data_buf[7]^crc_buf[31]^crc_buf[30]^crc_buf[29]^crc_buf[28]^crc_buf[27]^crc_buf[25]^crc_buf[24];
    crc_out[6]  = in_data_buf[0]^in_data_buf[1]^in_data_buf[2]^in_data_buf[3]^in_data_buf[5]^in_data_buf[6]^
                crc_buf[31]^crc_buf[30]^crc_buf[29]^crc_buf[28]^crc_buf[26]^crc_buf[25];
    crc_out[7]  = in_data_buf[0]^in_data_buf[2]^in_data_buf[4]^in_data_buf[5]^in_data_buf[7]^crc_buf[31]^
                crc_buf[29]^crc_buf[27]^crc_buf[26]^crc_buf[24];
    crc_out[8]  = in_data_buf[3]^in_data_buf[4]^in_data_buf[6]^in_data_buf[7]^crc_buf[28]^crc_buf[27]^
                crc_buf[25]^crc_buf[24]^crc_buf[0];
    crc_out[9]  = in_data_buf[2]^in_data_buf[3]^in_data_buf[5]^in_data_buf[6]^crc_buf[29]^crc_buf[28]^
                crc_buf[26]^crc_buf[25]^crc_buf[1];
    crc_out[10] = in_data_buf[2]^in_data_buf[4]^in_data_buf[5]^in_data_buf[7]^crc_buf[29]^crc_buf[27]^
                crc_buf[26]^crc_buf[24]^crc_buf[2];
    crc_out[11] = in_data_buf[3]^in_data_buf[4]^in_data_buf[6]^in_data_buf[7]^crc_buf[28]^crc_buf[27]^
                crc_buf[25]^crc_buf[24]^crc_buf[3];

    crc_out[12] = in_data_buf[1]^in_data_buf[2]^in_data_buf[3]^in_data_buf[5]^in_data_buf[6]^in_data_buf[7]^
                crc_buf[30]^crc_buf[29]^crc_buf[28]^crc_buf[26]^crc_buf[25]^crc_buf[24]^crc_buf[4];
    crc_out[13] = in_data_buf[0]^in_data_buf[1]^in_data_buf[2]^in_data_buf[4]^in_data_buf[5]^in_data_buf[6]^
                crc_buf[31]^crc_buf[30]^crc_buf[29]^crc_buf[27]^crc_buf[26]^crc_buf[25]^crc_buf[5];
    crc_out[14] = in_data_buf[0]^in_data_buf[1]^in_data_buf[3]^in_data_buf[4]^in_data_buf[5]^
                crc_buf[31]^crc_buf[30]^crc_buf[28]^crc_buf[27]^crc_buf[26]^crc_buf[6];
    crc_out[15] = in_data_buf[0]^in_data_buf[2]^in_data_buf[3]^in_data_buf[4]^crc_buf[31]^
                crc_buf[29]^crc_buf[28]^crc_buf[27]^crc_buf[7];
    crc_out[16] = in_data_buf[2]^in_data_buf[3]^in_data_buf[7]^crc_buf[29]^crc_buf[28]^crc_buf[24]^crc_buf[8];
    crc_out[17] = in_data_buf[1]^in_data_buf[2]^in_data_buf[6]^crc_buf[30]^crc_buf[29]^crc_buf[25]^crc_buf[9];
    crc_out[18] = in_data_buf[0]^in_data_buf[1]^in_data_buf[5]^crc_buf[31]^crc_buf[30]^crc_buf[26]^crc_buf[10];
    crc_out[19] = in_data_buf[0]^in_data_buf[4]^crc_buf[31]^crc_buf[27]^crc_buf[11];
    crc_out[20] = in_data_buf[3]^crc_buf[28]^crc_buf[12];
    crc_out[21] = in_data_buf[2]^crc_buf[29]^crc_buf[13];
    crc_out[22] = in_data_buf[7]^crc_buf[24]^crc_buf[14];
    crc_out[23] = in_data_buf[1]^in_data_buf[6]^in_data_buf[7]^crc_buf[30]^crc_buf[25]^crc_buf[24]^crc_buf[15];
    crc_out[24] = in_data_buf[0]^in_data_buf[5]^in_data_buf[6]^crc_buf[31]^crc_buf[26]^crc_buf[25]^crc_buf[16];
    crc_out[25] = in_data_buf[4]^in_data_buf[5]^crc_buf[27]^crc_buf[26]^crc_buf[17];
    crc_out[26] = in_data_buf[1]^in_data_buf[3]^in_data_buf[4]^in_data_buf[7]^crc_buf[30]^
                crc_buf[28]^crc_buf[27]^crc_buf[24]^crc_buf[18];
    crc_out[27] = in_data_buf[0]^in_data_buf[2]^in_data_buf[3]^in_data_buf[6]^crc_buf[31]^
                crc_buf[29]^crc_buf[28]^crc_buf[25]^crc_buf[19];
    crc_out[28] = in_data_buf[1]^in_data_buf[2]^in_data_buf[5]^crc_buf[30]^crc_buf[29]^crc_buf[26]^crc_buf[20];
    crc_out[29] = in_data_buf[0]^in_data_buf[1]^in_data_buf[4]^crc_buf[31]^crc_buf[30]^crc_buf[27]^crc_buf[21];
    crc_out[30] = in_data_buf[0]^in_data_buf[3]^crc_buf[31]^crc_buf[28]^crc_buf[22];
    crc_out[31] = in_data_buf[2]^crc_buf[23]^crc_buf[29];

    crc_result = 0;
    for (i = 0; i < 32; i++) { // 32:loop cap
        if (crc_out[i]) {crc_result |= (1 << i);}
    }

    return crc_result;
}

// #ifndef TLS_CONFIG_FPGA
u32 get_crc32(u8 *data, u32 data_size)
{
    u32 i;
    u32 val;
    int crc_result = 0xffffffff;

    for (i = 0; i < data_size; i++) {
        crc_result = _cal_crc32(crc_result, data[i]);
    }

    val = 0;
    for (i = 0; i < 32; i++) { // 32:loop cap
        if ((crc_result>>i) & 0x1) {
            val |= (1 << (31 - i)); // 31:byte alignment
        }
    }

    TLS_DBGPRT_INFO("calculate crc -0x%x .\n", ~val);
    return ~val;
}
// #endif

u32 checksum(u32 *data, u32 length, u32 init)
{
    static long long sum = 0;
    u32 checksum;
    u32 i;

    /*
        Calculate the checksum.
    */
    if (!init) {
        sum = 0;
    }

    for (i = 0; i < length; i++) {
        sum += *(data + i);
    }
    checksum = ~((u32)(sum >> 32) + (u32)sum); // 32:byte alignment

    return checksum;
}

int atodec(char ch)
{
    int dec = -1;

    if ((ch >= '0') && (ch <= '9')) {dec = ch - '0';}

    return dec;
}

int strtodec(int *dec, char *str)
{
    int i;
    int dd;
    int sign;
    char *strs_tmp = str;

    i = -1;
    dd = 0;
    sign = 1;

    if (*strs_tmp == '-') {
        strs_tmp++;
        sign = -1;
    }

    while (*strs_tmp) {
        i = atodec(*strs_tmp++);
        if (i < 0) {
            return -1;
        }
        dd = dd * 10 + i; // 10:byte alignment
    }

    *dec = dd*sign;

    return ((i < 0) ? -1 : 0);
}

int atohex(char ch)
{
    int hex;

    hex = -1;

    if ((ch >= '0') && (ch <= '9')) {
        hex = ch - '0';
    } else if ((ch >= 'a') && (ch <= 'f')) {
        hex = ch - 'a' + 0xa;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        hex = ch - 'A' + 0xa;
    }

    return hex;
}

int strtohex(u32 *hex, char *str)
{
    int n;
    int i;
    u32 dd;
    char *str_tmp = str;

    n = -1;
    i = 0;
    dd = 0;

    while (*str_tmp) {
        n = atohex(*str_tmp++);
        if (n < 0) {
            return -1;
        }
        dd = (dd << 4) + n; // 4:byte alignment
        if (++i > 8) { // 8:byte alignment
            return -1;
        }
    }

    *hex = dd;

    return (n < 0 ? -1 : 0);
}

int strtohexarray(u8 array[], int cnt, char *str)
{
    u8 tmp;
    u8 *des;
    int cnt_tmp = cnt;
    char *str_tmp = str;
    des = array;

    while (cnt_tmp-- > 0) {
        int hex = atohex(*str_tmp++);
        if (hex < 0) {
            return -1;
        } else {
            tmp = (hex << 4) & 0xf0; // 4:byte alignment
        }

        hex = atohex(*str_tmp++);
        if (hex < 0) {
            return -1;
        } else {
            tmp = tmp | (hex & 0x0f);
        }

        *des++ = (u8) tmp;
    }

    return ((*str_tmp==0) ? 0 : -1);
}

int strtoip(u32 *ipadr, char * str)
{
    int n;
    u32 i;
    u32 ip;
    char *head;
    char *tail;

    ip = 0;
    head = str;
    tail = str;

    for (i = 0; i < 3;) { // 3:byte alignment
        if (*tail == '.') {
            i++;
            *tail = 0;
            ip <<= 8; // 8:byte alignment
            if (strtodec(&n, head) < 0) {
                return -1;
            }
            if ((n < 0) || (n > 255)) { // 255:byte alignment
                return -1;
            }
            ip += n;
            *tail = '.';
            head = tail + 1;
        }
        tail++;
    }

    if (i < 3) { // 3:byte alignment
        return -1;
    }

    ip <<= 8; // 8:byte alignment
    if (strtodec(&n, head) < 0) {
        return -1;
    }
    if ((n < 0) || (n > 255)) { // 255:byte alignment
        return -1;
    }
    ip += n;

    *ipadr = ip;

    return ((ip == 0) ? -1 : 0);
}

void iptostr(u32 ip, char *str)
{
    int sret = sprintf(str, "%d.%d.%d.%d",
        ((ip >> 24) & 0xff), ((ip >> 16) & 0xff), // 24:byte alignment, 16:byte alignment
        ((ip >>  8) & 0xff), ((ip >>  0) & 0xff)); // 8:byte alignment
    if (sret < 0) {
        printf("sprintf error.\n");
    }
}

void mactostr(u8 mac[], char *str)
{
    int sret = sprintf(str, "%02x%02x%02x%02x%02x%02x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    if (sret < 0) {
        printf("sprintf error.\n");
    }
}

int hex_to_digit(int c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - ('A' - 10); // 10:byte alignment
    }
    if (c >= 'a' && c <= 'f') {
        return c - ('a' - 10); // 10:byte alignment
    }
    return -1;
}

int digit_to_hex(int c)
{
    if (c >= 0 && c <= 9) { // 9:byte alignment
        return c + '0';
    }
    if (c >= 0xA && c <= 0xF) {
        return c - 0xA + 'A' ;
    }
    return -1;
}

int hexstr_to_unit(char *buf, u32 *d)
{
    int i;
    int len = strlen(buf);
    *d = 0;

    if (len > 8) { // 8:byte alignment
        return -1;
    }
    for (i = 0; i < len; i++) {
        int c = hex_to_digit(buf[i]);
        if (c < 0) {
            return -1;
        }
        *d = (u8)c | (*d << 4); // 4:byte alignment
    }
    return 0;
}
int string_to_uint(char *buf, u32 *d)
{
    int i;
    int len = strlen(buf);
    if (len > 11 || len == 0) {  // 11:长度大于11或长度等于0
        return -1;
    }
    for (i = 0; i < len; i++) {
        if (!isdigit(buf[i])) {
            return -1;
        }
    }
    *d = atoi(buf);
    return 0;
}

int string_to_ipaddr(const char *buf, u8 *addr)
{
    int count = 0, rc = 0;
    unsigned int in[4];
    char c;

    rc = sscanf(buf, "%u.%u.%u.%u%c",
                &in[0], &in[1], &in[2], &in[3], &c);
    if (rc != 4 && (rc != 5 || c != '\n')) { // 4:sscanf的返回值不等于4, 5:sscanf的返回值不等于5
        return -1;
    }
    for (count = 0; count < 4; count++) {
        if (in[count] > 255) { // 255:数组值大于255
            return -1;
        }
        addr[count] = in[count];
    }
    return 0;
}

char *strdup(const char *s)
{
    char *ret;
    int len;
    len = strlen(s) + 1;
    ret = tls_mem_alloc(len);
    if (ret == NULL) {
        return NULL;
    }
    memset_s(ret, len, 0, len);
    memcpy_s(ret, len - 1, s, len - 1);
    return ret;
}

char *strndup(const char *s, size_t len)
{
    char *ret;
    ret = tls_mem_alloc(len + 1);
    if (ret == NULL) {
        return NULL;
    }
    memset_s(ret, len + 1, 0, len + 1);
    memcpy_s(ret, len, s, len);
    return ret;
}

void delay_cnt(int count)
{
#ifdef TLS_CONFIG_CPU_XT804
    volatile int delay = count;
#else
    int delay = count;
#endif

    while (delay--) {
    }
}

void dumpBuffer(char *name, char* buffer, int len)
{
#if 1
    int i = 0;
    printf("%s:\n", name);
    for (; i < len; i++) {
        printf("%02X, ", buffer[i]);
        if ((i + 1) % 16 == 0) { // 16:byte alignment
            printf("\n");
        }
    }
    printf("\n");
#endif
}
void dumpUint32(char *name, uint32_t* buffer, int len)
{
    int i = 0;
    printf("%s:\n", name);
    for (; i < len; i++) {
        printf("%08x ", buffer[i]);
        if ((i + 1) % 8 == 0) { // 8:byte alignment
            printf("\n");
        }
    }
    printf("\n");
}

int strcasecmp(const char *s1, const char *s2)
{
    const char *s1_tmp = s1;
    const char *s2_tmp = s2;
    while (*s1_tmp && *s2_tmp) {
        char a = *s1_tmp++;
        char b = *s2_tmp++;

        if (a == b) {
            continue;
        }

        if (a >= 'a' && a <= 'z') {
            a -= 'a' - 'A';
        }
        if (b >= 'a' && b <= 'z') {
            b -= 'a' - 'A';
        }

        if (a != b) {
            return 1;
        }
    }
    return 0;
}