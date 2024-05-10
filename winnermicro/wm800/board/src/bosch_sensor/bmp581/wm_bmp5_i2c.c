#include <stdio.h>
#include "wm_type_def.h"
#include "bmp5_defs.h"

int wm_bmp5_WriteOneByte(uint8_t dev_addr, uint16_t addr, uint8_t data)
{
    tls_i2c_write_byte(BMP5_I2C_ADDR_PRIM, 1);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(addr, 0);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(data, 0);
    tls_i2c_wait_ack();
    tls_i2c_stop();
    tls_os_time_delay(1);

    return WM_SUCCESS;
}


int wm_bmp5_read_i2c(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint32_t len)
{
    if(buf == NULL) {
        printf("%s: buf is NULL!\n", __func__);
        return WM_FAILED;
    }

    tls_i2c_write_byte(dev_addr, 1);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(addr,0);
    tls_i2c_wait_ack();
    tls_i2c_write_byte(dev_addr + 1, 1);
    tls_i2c_wait_ack();
    while(len > 1)
    {
        *buf++ = tls_i2c_read_byte(1,0);
        //printf("\nread byte=%x\n",*(buf - 1));
        len --;
    }
    *buf = tls_i2c_read_byte(0,1);
    //printf("\nread byte=%x\n",*buf);
    return WM_SUCCESS;
}

int wm_bmp5_write_i2c(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t value;

    if(buf == NULL) {
        printf("%s: buf is NULL!\n", __func__);
        return WM_FAILED;
    }

    while(len--)
    {
        value = wm_bmp5_WriteOneByte(dev_addr, addr, *buf);
        if(value == WM_SUCCESS) {
            addr++;
            buf++;
        } else {
          return WM_FAILED;
        }
    }
    return WM_SUCCESS;
}
