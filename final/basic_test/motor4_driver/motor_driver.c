/* based on the code provided by Josh Newans
https://github.com/joshnewans/ros_arduino_bridge
and adapted to use a raspberry pico instead an arduino
also I've replaced the L298N driver by the DRV8833 driver
Insted using interrupts, I've used raspbery pico's PIO to read the encoders
 */

/*****
 * Motor Driver using DRV8833 Motor Driver Circuit
 *****/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "hardware/pio.h"
#include "hardware/timer.h"
#include "quadrature_encoder.pio.h"
#include "motor_driver.h"
#include "commands.h"

#include "encoder_driver.h"

uint32_t pwm_set_freq_duty(uint slice_num, uint chan, uint32_t f, int d)
{
  uint32_t clock = 125000000;
  uint32_t divider16 = clock / f / 4096 + (clock % (f * 4096) != 0);
  if (divider16 / 16 == 0)
    divider16 = 16;
  uint32_t wrap = clock * 16 / divider16 / f - 1;
  pwm_set_clkdiv_int_frac(slice_num, divider16 / 16, divider16 & 0xF);
  pwm_set_wrap(slice_num, wrap);
  pwm_set_chan_level(slice_num, chan, wrap * d / 100);
  return wrap;
}

uint32_t pwm_get_wrap(uint slice_num)
{
  valid_params_if(PWM, slice_num >= 0 && slice_num < NUM_PWM_SLICES);
  return pwm_hw->slice[slice_num].top;
}

void pwm_set_duty(uint slice_num, uint chan, int d)
{
  pwm_set_chan_level(slice_num, chan, pwm_get_wrap(slice_num) * d / 100);
}

/*
typedef struct
{
  uint gpioForward;
  uint gpioBackward;
  uint slice;
  uint Fchan;
  uint Bchan;
  bool forward;
  uint speed;
  uint freq;
  uint resolution;
  bool on;
} BiMotor;
*/

void BiMotorInit(BiMotor *m, uint gpioForward, uint gpioBackward, uint freq)
{
  gpio_set_function(gpioForward, GPIO_FUNC_PWM);
  m->gpioForward = gpioForward;
  m->slice = pwm_gpio_to_slice_num(gpioForward);
  m->Fchan = pwm_gpio_to_channel(gpioForward);

  gpio_set_function(gpioBackward, GPIO_FUNC_PWM);
  m->gpioBackward = gpioBackward;
  m->Bchan = pwm_gpio_to_channel(gpioBackward);

  m->freq = freq;
  m->speed = 0;
  m->forward = true;
  m->resolution = pwm_set_freq_duty(m->slice, m->Fchan, m->freq, 0);
  pwm_set_duty(m->slice, m->Bchan, 0);
  m->on = false;
}

void BiMotorspeed(BiMotor *m, int s, bool forward)
{
  if (forward)
  {
    pwm_set_duty(m->slice, m->Bchan, 0);
    pwm_set_duty(m->slice, m->Fchan, s);
    m->forward = true;
  }
  else
  {
    pwm_set_duty(m->slice, m->Fchan, 0);
    pwm_set_duty(m->slice, m->Bchan, s);
    m->forward = true;
  }
  m->speed = s;
}

void BiMotorOn(BiMotor *m)
{
  pwm_set_enabled(m->slice, true);
  m->on = true;
}

void BiMotorOff(BiMotor *m)
{
  pwm_set_enabled(m->slice, false);
  m->on = false;
}

volatile uint64_t t1 = 0;
volatile uint64_t t2 = 0;

// Driver
BiMotor mot1; // LEFT
BiMotor mot2; // RIGHT

/* Wrap the motor driver initialization */
void initMotorController()
{

  BiMotorInit(&mot1, 3, 2, 2000);
  BiMotorInit(&mot2, 5, 4, 2000);
  const uint STBY_PIN = 6;

  gpio_init(STBY_PIN);
  gpio_set_dir(STBY_PIN, GPIO_OUT);
  gpio_put(STBY_PIN, 1);
}

/* Wrap the drive motor set speed function */
void setMotorSpeed(int i, int spd)
{
  if (i == LEFT)
  {
    if (spd >= 0)
      BiMotorspeed(&mot1, spd, false);
    else
      BiMotorspeed(&mot1, -spd, true);
  }
  else if (i == RIGHT)
  {
    if (spd >= 0)
      BiMotorspeed(&mot2, spd, false);
    else
      BiMotorspeed(&mot2, -spd, true);
  }
}

void setMotorSpeeds(int leftSpeed, int rightSpeed)
{
  // printf("setMotorSpeeds(%i, %i)\n", leftSpeed, rightSpeed);
  setMotorSpeed(LEFT, leftSpeed);
  setMotorSpeed(RIGHT, rightSpeed);
}
