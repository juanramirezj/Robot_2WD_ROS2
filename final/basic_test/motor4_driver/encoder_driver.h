/* based on the code provided by Josh Newans
https://github.com/joshnewans/ros_arduino_bridge
and adapted to use a raspberry pico instead an arduino
also I've replaced the L298N driver by the DRV8833 driver
Insted using interrupts, I've used raspbery pico's PIO to read the encoders
 */


/* *************************************************************
   Encoder driver function definitions - by James Nugen
   ************************************************************ */
   
// #define LEFT 1
// #define RIGHT 2

//State machine definitions
#define SM1  0
#define SM2  1

   
long readEncoder(int i);
void resetEncoder(int i);
void resetEncoders();

