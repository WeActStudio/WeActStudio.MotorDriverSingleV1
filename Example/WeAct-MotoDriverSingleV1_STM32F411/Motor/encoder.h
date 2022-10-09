/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#ifndef __ENCODER_H
#define __ENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "tim.h"

#define ENCODER_TIM_PERIOD (0xffff)
#define ENCODER_PER_COUNTER (360)
#define MOTOR_PER_COUNTER (1584)

    void encoder_base_init(void);
    uint32_t encoder_getCounter(void);
    void encoder_resetCounter(void);
    int16_t encoder_getOverflowCnt(void);
    void encoder_resetOverflowCnt(void);
    void encoder_handle(void);

#ifdef __cplusplus
}
#endif

#endif
