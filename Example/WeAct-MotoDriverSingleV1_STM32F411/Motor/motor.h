/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "tim.h"

#define MOTOR_FWD (0)
#define MOTOR_REV (1)
#define MOTOR_DECAYMODE_FAST (0)
#define MOTOR_DECAYMODE_SLOW (1)

#define MOTOR_DEFAUT_DRIVE_FREQ (10000)
#define MOTOR_DRIVE_MAX_CURRENT (3300)

#define MOTOR_CURRENT_SET(x) (x * 1000 / MOTOR_DRIVE_MAX_CURRENT) // 3300mA Max.

    void motor_base_init(void);
    void motor_setSpeed(uint16_t speed);
    void motor_setDir(uint8_t dir);
    void motor_setDecayMode(uint8_t mode);
    void motor_setCurrent(uint16_t current);
    void motor_setSpeedDir(uint16_t speed, uint8_t dir);

#ifdef __cplusplus
}
#endif

#endif
