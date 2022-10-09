/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#include "motor.h"

// Motor IN1   PA6
// Motor IN2   PA7
// Motor ISET  PB0

static uint8_t _motor_DecayMode = MOTOR_DECAYMODE_FAST;
static uint8_t _motor_DirSet = MOTOR_FWD;
static uint16_t _motor_DutySet = 0;
static uint16_t _motor_CurrentDutySet = MOTOR_CURRENT_SET(2000);

void motor_base_init(void)
{
    gpio_init_type gpio_init_struct;
    tmr_output_config_type tmr_oc_init_structure;

    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    /* get system clock */
    crm_clocks_freq_get(&crm_clocks_freq_struct);
      
    /* tmr3 clock enable */
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

    /* gpioa gpiob clock enable */
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_0;
    gpio_init(GPIOB, &gpio_init_struct);

    /* tmr3 time base configuration */
    tmr_base_init(TMR3, 1000 - 1, (crm_clocks_freq_struct.apb1_freq / MOTOR_DEFAUT_DRIVE_FREQ / 1000) - 1);
    tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
    tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);

    tmr_output_default_para_init(&tmr_oc_init_structure);
    tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
    tmr_oc_init_structure.oc_idle_state = FALSE;
    tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
    tmr_oc_init_structure.oc_output_state = TRUE;

    // Motor INA
    tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
    tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_1, TRUE);

    // Motor INB
    tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_2, &tmr_oc_init_structure);
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
    tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_2, TRUE);

    // Motor ISET
    tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_3, &tmr_oc_init_structure);
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, 999);
    tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_3, TRUE);

    tmr_period_buffer_enable(TMR3, TRUE);

    /* tmr enable counter */
    tmr_counter_enable(TMR3, TRUE);

    motor_setCurrent(2000);

    motor_setSpeedDir(0, MOTOR_FWD);
}

void motor_setSpeed(uint16_t speed)
{
    uint32_t temp1, temp2, temp3;

    if (speed > 1000)
        speed = 1000;

    if (speed == _motor_DutySet)
        return;

    // 读取定时器周期
    temp1 = tmr_period_value_get(TMR3);
    // 计算比较值
    _motor_DutySet = speed;
#if 0
    temp2 = temp1 * speed / 1000;
#else
    temp2 = speed;
#endif
    temp3 = temp1 - temp2;
    // 设置比较值
    if (!_motor_DirSet) // 正转
    {
        if (_motor_DecayMode) // 慢衰
        {
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, temp1);
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, temp3);
        }
        else
        {
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, temp2);
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
        }
    }
    else // 反转
    {
        if (_motor_DecayMode)
        {
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, temp3);
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, temp1);
        }
        else
        {
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
            tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, temp2);
        }
    }
}

void motor_setDir(uint8_t dir)
{
    uint32_t period, compare1, compare2;

    if (dir != _motor_DirSet)
    {
        period = tmr_period_value_get(TMR3);
        compare1 = tmr_channel_value_get(TMR3, TMR_SELECT_CHANNEL_1);
        compare2 = tmr_channel_value_get(TMR3, TMR_SELECT_CHANNEL_2);

        tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
        tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);

        _motor_DirSet = dir;
        if (!_motor_DirSet) // 正转
        {
            if (_motor_DecayMode) // 慢衰
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, compare1);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, period);
            }
            else
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, compare2);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
            }
        }
        else
        {
            if (_motor_DecayMode)
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, compare2);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, period);
            }
            else
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, compare1);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
            }
        }
    }
}

void motor_setDecayMode(uint8_t mode)
{
    uint32_t period, compare1, compare2;
    if (mode != _motor_DecayMode)
    {
        _motor_DecayMode = mode;

        period = tmr_period_value_get(TMR3);
        compare1 = tmr_channel_value_get(TMR3, TMR_SELECT_CHANNEL_1);
        compare2 = tmr_channel_value_get(TMR3, TMR_SELECT_CHANNEL_2);

        // 设置目标值
        if (!_motor_DirSet) // 正转
        {
            if (_motor_DecayMode)
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, period - compare1);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, period);
            }
            else
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, period - compare2);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0);
            }
        }
        else
        {
            if (_motor_DecayMode)
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, period - compare2);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, period);
            }
            else
            {
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, period - compare1);
                tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 0);
            }
        }
    }
}

void motor_setCurrent(uint16_t current)
{
    if (current > MOTOR_DRIVE_MAX_CURRENT)
        _motor_CurrentDutySet = MOTOR_CURRENT_SET(MOTOR_DRIVE_MAX_CURRENT);
    else
        _motor_CurrentDutySet = MOTOR_CURRENT_SET(current);

    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, _motor_CurrentDutySet);
}

void motor_setSpeedDir(uint16_t speed, uint8_t dir)
{
    motor_setSpeed(speed);
    motor_setDir(dir);
}
