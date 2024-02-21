/* based on the code provided by Josh Newans
https://github.com/joshnewans/ros_arduino_bridge
and adapted to use a raspberry pico instead an arduino
also I've replaced the L298N driver by the DRV8833 driver
Insted using interrupts, I've used raspbery pico's PIO to read the encoders
 */

#include <stdio.h>
#include "quadrature_encoder.pio.h"
#include "encoder_driver.h"
#include "commands.h"

/* Wrap the encoder reading function */
long readEncoder(int i)
{
  long e;
  if (i == LEFT)
    e = quadrature_encoder_get_count(pio0, SM1); 
  else
    e = -quadrature_encoder_get_count(pio0, SM2);//as deffault counts negative when going forward
  // printf("readEncoder(%i)=%ld\n", i, e);
  return e;
}

/* Wrap the encoder reset function */
void resetEncoder(int i)
{
  if (i == LEFT)
  {
    // not implemented yet
    return;
  }
  else
  {
    // not implemented yet
    return;
  }
}

/* Wrap the encoder reset function */
void resetEncoders()
{
  resetEncoder(LEFT);
  resetEncoder(RIGHT);
}
