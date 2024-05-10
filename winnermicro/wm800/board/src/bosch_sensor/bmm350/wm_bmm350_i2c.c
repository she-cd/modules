#include <stdio.h>
#include "wm_type_def.h"
#include "bmm350.h"
#define BMM350_DEV_ADDR 0xd2

int wm_bmm350_WriteOneByte(uint8_t dev_addr, uint16_t addr, uint8_t data)
{
    tls_i2c_write_byte(dev_addr, 1);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(addr, 0);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(data, 0);
    tls_i2c_wait_ack();
    tls_i2c_stop();
    tls_os_time_delay(1);

    return WM_SUCCESS;
}


int wm_bmm350_read_i2c(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint32_t len)
{
    tls_i2c_write_byte(dev_addr, 1);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(addr,0);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(dev_addr+1, 1);
    tls_i2c_wait_ack();
    while(len > 1)
    {
        *buf++ = tls_i2c_read_byte(1,0);
        printf("\nread byte=%x\n",*(buf - 1));
        len --;
    }
    *buf = tls_i2c_read_byte(0,1);
    printf("\nread byte=%x\n",*buf);

    return WM_SUCCESS;
}

int wm_bmm350_write_i2c(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t value;

    while(len--)
    {
        value = wm_bmm350_WriteOneByte(dev_addr, addr, *buf);
        if(value == WM_SUCCESS) {
            addr++;
            buf++;
        } else {
          return WM_FAILED;
        }
    }
    return WM_SUCCESS;
}
