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

/**
 * @file    wm_touchsensor.c
 *
 * @brief   touchsensor Driver Module
 *
 * @author
 *
 * Copyright (c) 2021 Winner Microelectronics Co., Ltd.
 */
#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_cpu.h"
#include "wm_gpio.h"

typedef void (*touchsensor_cb)(u32 status);
touchsensor_cb tc_callback = NULL;
/**
 * @brief          This function is used to initialize touch sensor.
 *
 * @param[in]      sensorno    is the touch sensor number from 1-15
 * @param[in]      scan_period is scan period for per touch sensor ,unit:16ms, >0
 * @param[in]      window      is count window, window must be greater than 2.Real count window is window - 2.
 * @param[in]      enable      is touch sensor enable bit.
 *
 * @retval         0:success
 *
 * @note           if use touch sensor, user must configure the IO multiplex by API wm_touch_sensor_config.
 */
int tls_touchsensor_init_config(u32 sensorno, u8 scan_period, u8 window, u32 enable)
{
    u32 regval = 0;

    regval = tls_reg_read32(HR_TC_CONFIG);

    /* firstly, disable scan function */
    tls_reg_write32(HR_TC_CONFIG, regval&(~(1<<TOUCH_SENSOR_EN_BIT)));

    if (scan_period >=0x3F) {
        regval &= ~(0x3F<<SCAN_PERID_SHIFT_BIT);
        regval |= (scan_period<<SCAN_PERID_SHIFT_BIT);
    }

    if (window) {
        regval &= ~(0x3F<<CAPDET_CNT_SHIFT_BIT);
        regval |= (window<<CAPDET_CNT_SHIFT_BIT);
    }

    if (sensorno && (sensorno <= 15)) {
        regval |= (1<<(sensorno-1+TOUCH_SENSOR_SEL_SHIFT_BIT));
    }

    if (enable) {
        regval |= (1<<TOUCH_SENSOR_EN_BIT);
    }

    tls_reg_write32(HR_TC_CONFIG, regval);

    return 0;
}

/**
 * @brief        This function is used to deinit touch sensor's selection and disable touch.
 *
 * @param[in]    sensorno    is the touch sensor number from 1-15
 *
 * @retval       0:success
 *
 * @note         if do not use touch sensor, user can deinit by this interface and configure this touch sensor as GPIO.
 */
int tls_touchsensor_deinit(u32 sensorno)
{
    u32 regval = 0;

    regval = tls_reg_read32(HR_TC_CONFIG);
    if (sensorno && (sensorno <= 15)) {
        regval &= ~(1<<(sensorno-1+TOUCH_SENSOR_SEL_SHIFT_BIT));
    }
    regval &= ~(1<<TOUCH_SENSOR_EN_BIT);
    tls_reg_write32(HR_TC_CONFIG, regval);

    return 0;
}

/**
 * @brief          This function is used to set threshold per touch sensor.
 *
 * @param[in]      sensorno    is the touch sensor number from 1-15
 * @param[in]      threshold   is the sensorno's touch sensor threshold,max value is 127.
 *
 * @retval         0:success. minus value: parameter wrong.
 *
 * @note           None
 */
int tls_touchsensor_threshold_config(u32 sensorno, u8 threshold)
{
    u32 regvalue = 0;
    if ((sensorno == 0) || (sensorno > 15)) {
        return -1;
    }

    if (threshold > 0x7F) {
        return -2;
    }

    regvalue = tls_reg_read32(HR_TC_CONFIG+sensorno*4);
    regvalue &= ~(0x7F);
    regvalue |= threshold;
    tls_reg_write32(HR_TC_CONFIG + sensorno*4, regvalue);
    return 0;
}

/**
 * @brief          This function is used to get touch sensor's count number.
 *
 * @param[in]      sensorno    is the touch sensor number from 1 to 15.
 *
 * @retval         sensorno's count number  .
 *
 * @note           None
 */
int tls_touchsensor_countnum_get(u32 sensorno)
{
    if ((sensorno == 0) || (sensorno > 15)) {
        return -1;
    }

    return ((tls_reg_read32(HR_TC_CONFIG+sensorno*4)>>8)&0x3FFF);
}

/**
 * @brief          This function is used to enable touch sensor's irq.
 *
 * @param[in]      sensorno    is the touch sensor number  from 1 to 15.
 *
 * @retval         0:successfully enable irq, -1:parameter wrong.
 *
 * @note           None
 */
int tls_touchsensor_irq_enable(u32 sensorno)
{
    u32 value = 0;
    if (sensorno && (sensorno <= 15)) {
        value = tls_reg_read32(HR_TC_INT_EN);
        value |= (1<<(sensorno+15));
        tls_reg_write32(HR_TC_INT_EN, value);
        tls_irq_enable(TOUCH_IRQn);
        return 0;
    }

    return -1;
}

/**
 * @brief          This function is used to disable touch sensor's irq.
 *
 * @param[in]      sensorno    is the touch sensor number  from 1 to 15.
 *
 * @retval         0:successfully disable irq, -1:parameter wrong.
 *
 * @note           None
 */
int tls_touchsensor_irq_disable(u32 sensorno)
{
    u32 value = 0;
    if (sensorno && (sensorno <= 15)) {
        value = tls_reg_read32(HR_TC_INT_EN);
        value &= ~(1<<(sensorno+15));
        tls_reg_write32(HR_TC_INT_EN, value);
        if ((value & 0xFFFF0000) == 0) {
            tls_irq_disable(TOUCH_IRQn);
        }
        return 0;
    }

    return -1;
}

/**
 * @brief          This function is used to register touch sensor's irq callback.
 *
 * @param[in]      callback    is call back for user's application.
 *
 * @retval         None.
 *
 * @note           None
 */
void tls_touchsensor_irq_register(void (*callback)(u32 status))
{
    tc_callback = callback;
}

/**
 * @brief          This function is touch sensor's irq handler.
 *
 * @param[in]      None
 *
 * @retval         None
 *
 * @note           None
 */
ATTRIBUTE_ISR void tls_touchsensor_irq_handler(void)
{
    u32 value = 0;
    csi_kernel_intrpt_enter();
    value = tls_reg_read32(HR_TC_INT_EN);
    if (tc_callback) {
        tc_callback(value&0xFFFF);
    }
    tls_reg_write32(HR_TC_INT_EN, value);
    csi_kernel_intrpt_exit();
}

/**
 * @brief          This function is used to get touch sensor's irq status.
 *
 * @param[in]      sensorno    is the touch sensor number  from 1 to 15.
 *
 * @retval         >=0:irq status, -1:parameter wrong.
 *
 * @note           None
 */
int tls_touchsensor_irq_status_get(u32 sensorno)
{
    u32 value = 0;

    if (sensorno && (sensorno <= 15)) {
        value = tls_reg_read32(HR_TC_INT_EN);
        return (value&(1<<(sensorno-1)))?1:0;
    }

    return -1;
}

