/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#include "encoder.h"

// Motor Encoder CHA   PB6
// Motor Encoder CHB   PB7

static int16_t _encoder_OverflowCnt = -1;

void encoder_base_init(void)
{
    gpio_init_type gpio_init_struct;

    crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOB, &gpio_init_struct);

    /* tmr4 encoder mode configuration
     tmr4 ti1pf1 ,ti2fp2 as encoder input pin, tmr4 counter
     changed each signal edge. */

    tmr_base_init(TMR4, 0xFFFF, 0);
    tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);
    tmr_counter_value_set(TMR4, 0);

    /* config encoder mode */
    tmr_encoder_mode_config(TMR4, TMR_ENCODER_MODE_C, TMR_INPUT_RISING_EDGE, TMR_INPUT_RISING_EDGE);

    /* overflow interrupt enable */
    tmr_interrupt_enable(TMR4, TMR_OVF_INT, TRUE);

    /* tmr1 overflow interrupt nvic init */
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR4_GLOBAL_IRQn, 4, 0);

    /* enable tmr4 */
    tmr_counter_enable(TMR4, TRUE);
}

uint32_t encoder_getCounter(void)
{
    return tmr_counter_value_get(TMR4);
}

void encoder_resetCounter(void)
{
    tmr_counter_value_set(TMR4, 0);
}

int16_t encoder_getOverflowCnt(void)
{
    return _encoder_OverflowCnt;
}

void encoder_resetOverflowCnt(void)
{
    _encoder_OverflowCnt = 0;
}

void TMR4_GLOBAL_IRQHandler(void)
{
    if (tmr_flag_get(TMR4, TMR_OVF_FLAG) != RESET)
    {
        if (TMR4->ctrl1_bit.cnt_dir == TMR_COUNT_UP)
            _encoder_OverflowCnt++;
        else
            _encoder_OverflowCnt--;
        tmr_flag_clear(TMR4, TMR_OVF_FLAG);
    }
}
