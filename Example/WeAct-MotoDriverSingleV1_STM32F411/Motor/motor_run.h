/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#ifndef __MOTOR_RUN_H
#define __MOTOR_RUN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

#define MOTOR_RUN_MODE_SPEED (0)
#define MOTOR_RUN_MODE_LOC (1)

    void motor_run_base_init(void);
    void motor_run_setMode(uint8_t mode);
    void motor_run_setLocModeLoc(int32_t angle);
    void motor_run_setLocModeMaxSpeed(uint32_t max_speed);
    void motor_run_setSpeedModeSpeed(int32_t speed);
    int32_t motor_run_getSpeed(void);
    int32_t motor_run_getEncoderCnt(void);
    int32_t motor_run_getEncoderAngle(void);
    void motor_run_handle(void);

#ifdef __cplusplus
}
#endif

#endif
