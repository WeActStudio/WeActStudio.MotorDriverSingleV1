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
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  HAL_TIM_Base_Start_IT(&htim4);
}

uint32_t encoder_getCounter(void)
{
  return __HAL_TIM_GetCounter(&htim4);
}

void encoder_resetCounter(void)
{
  __HAL_TIM_SetCounter(&htim4, 0);
}

int16_t encoder_getOverflowCnt(void)
{
  return _encoder_OverflowCnt;
}

void encoder_resetOverflowCnt(void)
{
  _encoder_OverflowCnt = 0;
}

void encoder_handle(void)
{
  if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4))
  {
    _encoder_OverflowCnt--;
  }
  else
  {
    _encoder_OverflowCnt++;
  }
}
