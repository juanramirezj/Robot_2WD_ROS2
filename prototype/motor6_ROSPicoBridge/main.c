#include <stdio.h>

// #include <rcl/rcl.h>
// #include <rcl/error_handling.h>
// #include <rclc/rclc.h>
// #include <rclc/executor.h>
// #include <std_msgs/msg/int32.h>
// #include <nav_msgs/msg/odometry.h>
// #include <geometry_msgs/msg/twist.h>
// #include <rmw_microros/rmw_microros.h>

#include "motor_driver.h"

#include "pico/stdlib.h"
// #include "pico_uart_transports.h"

#include "ssd1306.h"  // https://github.com/daschr/pico-ssd1306

#include "motor.h"
#include "diff_controller.h"
#include "commands.h"

#include <inttypes.h>

#define STRING_BUFFER_LEN 100

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


const uint LED_PIN = 25;
const uint ENCODER1_PIN = 26;
const uint ENCODER2_PIN = 27;

rcl_publisher_t publisher;
rcl_subscription_t ping_subscriber;

std_msgs__msg__Int32 msg;
geometry_msgs__msg__Twist incoming_ping;

// Variables to test motor
// https://github.com/joshnewans/ros_arduino_bridge/blob/main/ROSArduinoBridge/ROSArduinoBridge.ino

// Used part of the code:
// https://github.com/joshnewans/ros_arduino_bridge/tree/main


/* Run the PID loop at 30 times per second */
#define PID_RATE           30     // Hz
/* Convert the rate into an interval */
const int PID_INTERVAL = 1000 / PID_RATE;

/* Track the next time we make a PID calculation */
unsigned long nextPID = PID_INTERVAL;

/* Stop the robot if it hasn't received a movement command
in this number of milliseconds */
#define AUTO_STOP_INTERVAL 5000
long lastMotorCommand = AUTO_STOP_INTERVAL;

// A pair of varibles to help parse serial commands (thanks Fergs)
int arg = 0;
int Myindex = 0;

// Variable to hold an input character
char chr;

// Variable to hold the current single-character command
char cmd;

// Character arrays to hold the first and second arguments
char argv1[16];
char argv2[16];

// The arguments converted to integers
long arg1;
long arg2;

/* Clear the current command parameters */
void resetCommand() {
  // cmd = NULL;
  cmd = '\0';
  memset(argv1, 0, sizeof(argv1));
  memset(argv2, 0, sizeof(argv2));
  arg1 = 0;
  arg2 = 0;
  arg = 0;
  Myindex = 0;
}

/* Run a command.  Commands are defined in commands.h */
int runCommand() {
  char line[20];
  int i = 0;
  char *p = argv1;
  char *str;
  int pid_args[4];
  arg1 = atoi(argv1);
  arg2 = atoi(argv2);
  sprintf(line,"%c", cmd);

  debug_txt(line);

  switch(cmd) {

  case READ_ENCODERS:
    printf("L=%" PRIu64 " R=%" PRIu64 "\n\r", readEncoder(LEFT), readEncoder(RIGHT));
    break;

   case RESET_ENCODERS:
    resetEncoders();
    resetPID();
    printf("OK\n\r");
    break;

  case MOTOR_SPEEDS:
    /* Reset the auto stop timer */
    lastMotorCommand = to_ms_since_boot(	get_absolute_time ());
    if (arg1 == 0 && arg2 == 0) {
      setMotorSpeeds(0, 0);
      resetPID();
      moving = 0;
    }
    else moving = 1;
    leftPID.TargetTicksPerFrame = arg1;
    rightPID.TargetTicksPerFrame = arg2;
    printf("OK\n\r"); 
    break;
  case MOTOR_RAW_PWM:
    /* Reset the auto stop timer */
    lastMotorCommand = to_ms_since_boot(	get_absolute_time ());
    resetPID();
    moving = 0; // Sneaky way to temporarily disable the PID
    setMotorSpeeds(arg1, arg2);
    printf("OK\n\r"); 
    break;
  case UPDATE_PID:
    while ((str = strtok_r(p, ":", &p)) !=  NULL) {
       pid_args[i] = atoi(str);
       i++;
    }
    Kp = pid_args[0];
    Kd = pid_args[1];
    Ki = pid_args[2];
    Ko = pid_args[3];
    printf("OK\n\r");
    break;

  default:
    printf("Invalid Command\n\r");
    break;
  }
}


void my_loop()
{
  int16_t chr;

  while(true)
  {
    chr = getchar_timeout_us(0);
    while (chr != PICO_ERROR_TIMEOUT) 
    {
      
      // Read the next character
      //chr = Serial.read();
      //chr = getchar();

      // Terminate a command with a CR
      printf("%c", chr);
      if (chr == 13) 
      {
        if (arg == 1) argv1[Myindex] = '\0';  //NULL;
        else if (arg == 2) argv2[Myindex] = '\0'; //NULL;
        runCommand();
        resetCommand();
      }
      // Use spaces to delimit parts of the command
      else if (chr == ' ') 
      {
        // Step through the arguments
        //printf("arg=%i Myindex=%i", arg, Myindex);
        if (arg == 0) arg = 1;
        else if (arg == 1)  
        {
          argv1[Myindex] = '\0';  //NULL;
          arg = 2;
          Myindex = 0;
        }
        chr = getchar_timeout_us(0);
        continue;
      }
      else 
      {
        if (arg == 0) 
        {
          // The first arg is the single-letter command
          cmd = chr;
        }
        else if (arg == 1) 
        {
          // Subsequent arguments can be more than one character
          argv1[Myindex] = chr;
          Myindex++;
        }
        else if (arg == 2) 
        {
          argv2[Myindex] = chr;
          Myindex++;
        }
      }
      chr = getchar_timeout_us(0);
    }
    
      // If we are using base control, run a PID calculation at the appropriate intervals

    if (to_ms_since_boot(	get_absolute_time ()) > nextPID) {
      updatePID();
      nextPID += PID_INTERVAL;

      char line[50];
      sprintf(line, "S=%i %i", speed1,speed2 );
      debug_txt(line);
    }
    
    // Check to see if we have exceeded the auto-stop interval
    if ((to_ms_since_boot(	get_absolute_time ()) - lastMotorCommand) > AUTO_STOP_INTERVAL) {;
      setMotorSpeeds(0, 0);
      moving = 0;
    }
  }  
}


void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    rcl_ret_t ret = rcl_publish(&publisher, &msg, NULL);
    msg.data++;
}



void ping_subscription_callback(const void * msgin)
{
    static int led_status = false;
	const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;
    char line[80];

	// Dont pong my own pings
    /*
	if(strcmp(outcoming_ping.frame_id.data, msg->frame_id.data) != 0){
		printf("Ping received with seq %s. Answering.\n", msg->frame_id.data);
		rcl_publish(&pong_publisher, (const void*)msg, NULL);
	}
    */
   // printf("Msg received with x=%i,y=%i,z=%i", msg->linear.x, msg->linear.y, msg->linear.z);
   led_status = !led_status;  
   if( led_status)
        gpio_put(LED_PIN, 1);
    else
        gpio_put(LED_PIN, 0);

    int speed;
    int forward;

    speed = msg->linear.x;
    sprintf(line,"S=%i", speed );
    debug_txt(line);

    forward = (speed >= 0);

    BiMotorspeed(&mot1, abs(speed), forward);
    BiMotorspeed(&mot2, abs(speed), forward); 
}

void blink_led(int n)
{
    for(int i=0; i<n; i++)
    {
        gpio_put(LED_PIN,1);
        sleep_ms(250);
        gpio_put(LED_PIN,0);
        sleep_ms(250);
    }
    sleep_ms(1000);
}


ssd1306_t disp;

void display_init()
{
    i2c_init(i2c0, 400000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);
}

void  debug_txt(char * text)
{
            ssd1306_clear(&disp);
            ssd1306_draw_string(&disp, 8, 24, 2, text);
            ssd1306_show(&disp);

}

int main()
{
    // Initilize

    resetPID();
    stdio_init_all();

    // Timer
    struct repeating_timer timer;
    add_repeating_timer_ms(delay_ms_timer, MyTimerHandler, NULL, &timer);

    //Interrupts
    gpio_set_function(mot1_speed, GPIO_FUNC_SIO);
    gpio_set_function(mot2_speed, GPIO_FUNC_SIO);
    gpio_set_dir(mot1_speed, false);
    gpio_pull_down(mot1_speed);
    gpio_set_dir(mot2_speed, false);
    gpio_pull_down(mot2_speed);

    gpio_set_irq_enabled_with_callback( mot1_speed, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &MyIRQHandler);
    gpio_set_irq_enabled_with_callback( mot2_speed, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &MyIRQHandler);

    BiMotor mot1;
    BiMotor mot2;
    BiMotorInit(&mot1, 3, 2, 2000);
    BiMotorInit(&mot2, 4, 5, 2000);
    const uint STBY_PIN = 6;


    gpio_init(STBY_PIN);
    gpio_set_dir(STBY_PIN, GPIO_OUT);
    gpio_put(STBY_PIN, 1);
    
    BiMotorOn(&mot1);
    BiMotorOn(&mot2);

    BiMotorspeed(&mot1, 100, true);
    BiMotorspeed(&mot2, 100, true);   
    sleep_ms(1000);
    printf("Speed1, Speed2");
    BiMotorspeed(&mot1, 0, false);
    BiMotorspeed(&mot2, 0, false);   

    // main loop    
    while( true)
    {
           
      // Read the next character
      chr = Serial.read();
      if (cmd == 'e') 
      {
          readEncoders(); // Call function to read encoders
      } 
      else if (cmd == 'm') 
      {
          int motor1Speed, motor2Speed;
          scanf("%d %d", &motor1Speed, &motor2Speed); // Read two integers from serial
          setMotorSpeed(motor1Speed, motor2Speed); // Call function to set motor speed
      }

      // Wait for Enter (newline) to process next command
      while (getchar() != '\n') {
          // Do nothing, just wait for newline character

    }
   

  if (millis() > nextPID) 
  {
    updatePID();
    nextPID += PID_INTERVAL;
  }
  
  // Check to see if we have exceeded the auto-stop interval
  if ((millis() - lastMotorCommand) > AUTO_STOP_INTERVAL) 
  {
    setMotorSpeeds(0, 0);
    moving = 0;
  }

}
