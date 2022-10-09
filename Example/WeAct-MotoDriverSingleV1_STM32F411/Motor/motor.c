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
    __HAL_TIM_SetAutoreload(&htim3,1000-1);
    __HAL_TIM_SET_PRESCALER(&htim3,(HAL_RCC_GetPCLK1Freq()/ MOTOR_DEFAUT_DRIVE_FREQ / 1000) - 1);
    __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,0);
    __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
    __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,999);
    __HAL_TIM_SetCounter(&htim3,0);
    
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

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
    temp1 = __HAL_TIM_GetAutoreload(&htim3);
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
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,temp1);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,temp3);
        }
        else
        {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,temp2);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
        }
    }
    else // 反转
    {
        if (_motor_DecayMode)
        {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,temp3);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,temp1);
        }
        else
        {
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,0);
            __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,temp2);
        }
    }
}

void motor_setDir(uint8_t dir)
{
    uint32_t period, compare1, compare2;

    if (dir != _motor_DirSet)
    {
        period = __HAL_TIM_GetAutoreload(&htim3);
        compare1 = __HAL_TIM_GetCompare(&htim3, TIM_CHANNEL_1);
        compare2 = __HAL_TIM_GetCompare(&htim3, TIM_CHANNEL_2);

        __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
        __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,0);

        _motor_DirSet = dir;
        if (!_motor_DirSet) // 正转
        {
            if (_motor_DecayMode) // 慢衰
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,compare1);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,period);
            }
            else
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,compare2);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
            }
        }
        else
        {
            if (_motor_DecayMode)
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,compare2);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,period);
            }
            else
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,compare1);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,0);
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

        period = __HAL_TIM_GetAutoreload(&htim3);
        compare1 = __HAL_TIM_GetCompare(&htim3, TIM_CHANNEL_1);
        compare2 = __HAL_TIM_GetCompare(&htim3, TIM_CHANNEL_2);

        // 设置目标值
        if (!_motor_DirSet) // 正转
        {
            if (_motor_DecayMode)
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,period - compare1);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,period);
            }
            else
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,period - compare2);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
            }
        }
        else
        {
            if (_motor_DecayMode)
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,period - compare2);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,period);
            }
            else
            {
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,period - compare1);
                __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,0);
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

    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, _motor_CurrentDutySet);
}

void motor_setSpeedDir(uint16_t speed, uint8_t dir)
{
    motor_setSpeed(speed);
    motor_setDir(dir);
}
