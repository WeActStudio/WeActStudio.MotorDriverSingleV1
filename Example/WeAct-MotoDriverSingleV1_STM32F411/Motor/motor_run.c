/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#include "motor_run.h"

#include "motor.h"
#include "encoder.h"
#include "pid.h"

static void motor_run_timer_base_init(void);
static uint32_t _motor_run_tick, _motor_run_encoder_tick;
static float _motor_loc_max_speed;
static uint8_t _motor_run_mode = 0;

int32_t _encoder_counter;
static int32_t _encoder_counter_last;
static int32_t _encoder_error;
static float _encoder_speed;
int32_t _encoder_speed1;

void motor_run_base_init(void)
{
    _motor_run_tick = 0;
    _motor_run_encoder_tick = 0;

    motor_run_setLocModeMaxSpeed(500);

    motor_run_setSpeedModeSpeed(0);

    motor_run_setMode(MOTOR_RUN_MODE_SPEED);

    PID_param_init();

    motor_base_init();

    encoder_base_init();

    motor_run_timer_base_init();

    encoder_resetOverflowCnt();
}

void motor_run_setLocModeMaxSpeed(uint32_t max_speed)
{
    _motor_loc_max_speed = max_speed * 4;
}

void motor_run_setSpeedModeSpeed(int32_t speed)
{
    if (speed > 25)
    {
        pid_speed.Kp = 1.3;
        pid_speed.Ki = 0.15;
    }
    else if (speed < -25)
    {
        pid_speed.Kp = 1.3;
        pid_speed.Ki = 0.15;
    }
    else
    {
        pid_speed.Kp = 3;
        pid_speed.Ki = 0.22;
    }
    set_pid_target(&pid_speed, speed * 4);
}

void motor_run_setLocModeLoc(int32_t angle)
{
    set_pid_target(&pid_location, angle * MOTOR_PER_COUNTER / 900);
}

void motor_run_setMode(uint8_t mode)
{
    _motor_run_mode = mode;
}

int32_t motor_run_getSpeed(void)
{
    return _encoder_speed1 / 4;
}

int32_t motor_run_getEncoderCnt(void)
{
    return _encoder_counter;
}

int32_t motor_run_getEncoderAngle(void)
{
    return _encoder_counter * 3600 / MOTOR_PER_COUNTER;
}

static void motor_run_timer_base_init(void)
{
    __HAL_TIM_SetAutoreload(&htim11, 10 - 1);
    __HAL_TIM_SET_PRESCALER(&htim11, (HAL_RCC_GetPCLK2Freq() / 1000 / 10) - 1);
    __HAL_TIM_SetCounter(&htim11, 0);
    HAL_TIM_Base_Start_IT(&htim11);
}

static void motor_run_encoder_calculate(void)
{
    // period 10ms
    uint32_t encoder_CounterNow = encoder_getCounter();

    _encoder_counter = (encoder_CounterNow + encoder_getOverflowCnt() * ENCODER_TIM_PERIOD);
    _encoder_error = _encoder_counter - _encoder_counter_last;

    if (_encoder_counter > 50000 * MOTOR_PER_COUNTER)
    {
        encoder_resetOverflowCnt();
        _encoder_counter = encoder_CounterNow;
    }

    _encoder_speed = (float)_encoder_error * 1000 * 60 / (float)MOTOR_PER_COUNTER;
    _encoder_speed1 = _encoder_speed;
    _encoder_counter_last = _encoder_counter;
}

static void speed_pid_run(float actual_speed)
{
    int32_t result_speed;
    uint8_t dir;

    result_speed = (int16_t)speed_pid_realize(&pid_speed, actual_speed);

    if (result_speed > 0)
    {
        dir = MOTOR_REV;
    }
    else
    {
        dir = MOTOR_FWD;
        result_speed = -result_speed;
    }
    if (result_speed > 1000)
        result_speed = 1000;
    motor_setSpeedDir(result_speed, dir);
}

static void speed_val_protect(float *speed_val)
{
    if (*speed_val > _motor_loc_max_speed)
    {
        *speed_val = _motor_loc_max_speed;
    }
    else if (*speed_val < -_motor_loc_max_speed)
    {
        *speed_val = -_motor_loc_max_speed;
    }
}

static void location_pid_run(void)
{
    float speed_val = location_pid_realize(&pid_location, _encoder_counter);
    speed_val_protect(&speed_val);
    set_pid_target(&pid_speed, speed_val);
}

void motor_run_handle(void)
{
    _motor_run_tick++;

    motor_run_encoder_calculate();

    if (_motor_run_tick > _motor_run_encoder_tick)
    {
        if (_motor_run_tick > 10000)
        {
            _motor_run_tick = 0;
        }
        _motor_run_encoder_tick = _motor_run_tick + 10;

        // location pid
        if (_motor_run_mode)
            location_pid_run();
    }

    // speed pid
    speed_pid_run(_encoder_speed);
}
