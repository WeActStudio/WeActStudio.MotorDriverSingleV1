/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

// Motor IN1   PA6
// Motor IN2   PA7
// Motor ISET  PB0
// Motor Encoder CHA   PB6
// Motor Encoder CHB   PB7

#include "at32f403a_407_clock.h"
#include "delay.h"
#include "board.h"

#include "motor_run.h"

#include "motor.h"
#include "encoder.h"
#include "pid.h"

int32_t motor_speed_test;
uint8_t motor_flag;
/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void)
{
  system_clock_config();

  delay_init();

  button_init();
  button_exint_init();
  led_init();

  led_init();

#if 0
  encoder_base_init();
  motor_base_init();
  
  motor_setCurrent(2000);
  motor_setSpeed(500);
#else
  motor_run_base_init();

  motor_setCurrent(2000);

  motor_run_setMode(MOTOR_RUN_MODE_SPEED);
  motor_run_setSpeedModeSpeed(100);
  delay_ms(1000);
#endif

  motor_speed_test = 0;
  motor_flag = 0;
  while (1)
  {
    led_toggle();
    delay_ms(g_speed * DELAY);

#if 1
    if (motor_speed_test > 1350)
    {
      motor_flag = 1;
      delay_ms(1000);
    }
    if (motor_speed_test < -1350)
    {
      motor_flag = 0;
      delay_ms(1000);
    }

    if (motor_flag)
      motor_speed_test -= 50;
    else
      motor_speed_test += 50;

    motor_run_setSpeedModeSpeed(motor_speed_test);

    if (motor_speed_test == 0)
      delay_ms(500);
#endif
  }
}

/**
 * @}
 */

/**
 * @}
 */
