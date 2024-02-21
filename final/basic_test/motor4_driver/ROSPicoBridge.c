
/* based on the code provided by Josh Newans
https://github.com/joshnewans/ros_arduino_bridge
and adapted to use a raspberry pico instead an arduino
also I've replaced the L298N driver by the DRV8833 driver
Insted using interrupts, I've used raspbery pico's PIO to read the encoders
 */

#include <stdlib.h>
#include <hardware/timer.h>
#include "encoder_driver.h"
/* Include definition of serial commands */
#include "commands.h"
#include <tusb.h>
#include "ROSPicoBridge.h"
#include "motor_driver.h"
#include "diff_controller.h"
#include "quadrature_encoder.pio.h"

/* Convert the rate into an interval */
const int PID_INTERVAL = 1000 / PID_RATE;

/* Track the next time we make a PID calculation */
unsigned long nextPID = PID_INTERVAL;

/* Stop the robot if it hasn't received a movement command
 in this number of milliseconds */
#define AUTO_STOP_INTERVAL 2000
long lastMotorCommand = AUTO_STOP_INTERVAL;

// A pair of varibles to help parse serial commands (thanks Fergs)
int arg = 0;
int myindex = 0;

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
void resetCommand()
{
    cmd = '\0';
    memset(argv1, 0, sizeof(argv1));
    memset(argv2, 0, sizeof(argv2));
    arg1 = 0;
    arg2 = 0;
    arg = 0;
    myindex = 0;
}

/* Run a command.  Commands are defined in commands.h */
int runCommand()
{
    int i = 0;
    char *p = argv1;
    char *str;
    int pid_args[4];
    arg1 = atoi(argv1);
    arg2 = atoi(argv2);

    switch (cmd)
    {
    case GET_BAUDRATE:
        printf("baudrate=%i", BAUDRATE);
        break;
    case ANALOG_READ:
        // printf(analogRead(arg1));
        break;
    case DIGITAL_READ:
        // printf(digitalRead(arg1));
        break;
    case ANALOG_WRITE:
        // analogWrite(arg1, arg2);
        // printf("OK");
        break;
    case DIGITAL_WRITE:
        /*
          if (arg2 == 0) digitalWrite(arg1, LOW);
          else if (arg2 == 1) digitalWrite(arg1, HIGH);
          printf("OK");
          */
        break;
    case PIN_MODE:
        /*
        if (arg2 == 0)
            pinMode(arg1, INPUT);
        else if (arg2 == 1)
            pinMode(arg1, OUTPUT);
        printf("OK");
        */
        break;
    case PING:
        // printf(Ping(arg1));
        break;

    case READ_ENCODERS:
        printf("left encoder=%i right encoder=%i\n", readEncoder(LEFT), readEncoder(RIGHT));

        break;
    case RESET_ENCODERS:
        resetEncoders();
        resetPID();
        printf("OK");
        break;
    case MOTOR_SPEEDS:
        /* Reset the auto stop timer */
        lastMotorCommand = to_ms_since_boot(get_absolute_time());
        if (arg1 == 0 && arg2 == 0)
        {
            setMotorSpeeds(0, 0);
            resetPID();
            moving = 0;
        }
        else
            moving = 1;
        printf("Move command rcvd left=%i right=%i\n", arg1, arg2);
        leftPID.TargetTicksPerFrame = arg1;
        rightPID.TargetTicksPerFrame = arg2;
        printf("OK");
        break;
    case MOTOR_RAW_PWM:
        /* Reset the auto stop timer */
        lastMotorCommand = to_ms_since_boot(get_absolute_time());
        resetPID();
        moving = 0; // Sneaky way to temporarily disable the PID
        printf("Move command rcvd left=%i right=%i\n", arg1, arg2);
        setMotorSpeeds(arg1, arg2);
        printf("OK");
        break;
    case UPDATE_PID:
        while ((str = strtok_r(p, ":", &p)) != '\0')
        {
            pid_args[i] = atoi(str);
            i++;
        }
        Kp = pid_args[0];
        Kd = pid_args[1];
        Ki = pid_args[2];
        Ko = pid_args[3];
        printf("OK");
        printf("PID updated Kp=%i Kd=%i Ki=%i Ko=%i\n", Kp, Kd, Ki, Ko);
        break;

    default:
        printf("Invalid Command");
        break;
    }
}

void main()
{

    int new_value1, delta1, old_value1 = 0;
    int new_value2, delta2, old_value2 = 0;

    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    // Note: I used gpp26 and gp27 for second motor, but for some reasong it never worked. I change to GP18 and GP19 and it worked. GP20 and GP21 also worked.
    const uint PIN_AB1 = 18;
    const uint PIN_AB2 = 20;

    stdio_init_all();

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    // using states machine 0 and 1 from pio0
    pio_add_program(pio0, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio0, SM1, PIN_AB1, 0);
    quadrature_encoder_program_init(pio0, SM2, PIN_AB2, 0);

    BiMotor mot1;
    BiMotor mot2;
    BiMotorInit(&mot1, 3, 2, 2000);
    BiMotorInit(&mot2, 5, 4, 2000);
    const uint STBY_PIN = 6;

    gpio_init(STBY_PIN);
    gpio_set_dir(STBY_PIN, GPIO_OUT);
    gpio_put(STBY_PIN, 1);

    BiMotorOn(&mot1);
    BiMotorOn(&mot2);

    BiMotorspeed(&mot1, 100, true);
    BiMotorspeed(&mot2, 100, true);

    initMotorController();
    resetPID();

    while (!tud_cdc_connected())
    {
        sleep_ms(100);
    }

    BiMotorspeed(&mot1, 100, true);
    BiMotorspeed(&mot2, 100, true);
    sleep_ms(500);
    BiMotorspeed(&mot1, 100, false);
    BiMotorspeed(&mot2, 100, false);
    sleep_ms(500);
    BiMotorspeed(&mot1, 0, false);
    BiMotorspeed(&mot2, 0, false);


    printf("PID parameters Kp=%i Kd=%i Ki=%i Ko=%i\n", Kp, Kd, Ki, Ko);

    printf("OK. Motor controlled connected. Ready to receive commands\n");
    while (true)
    {
        while (tud_cdc_available())
        {

            // Read the next character
            chr = getchar();

            // Terminate a command with a CR
            if (chr == 13)
            {
                if (arg == 1)
                    argv1[myindex] = '\0';
                else if (arg == 2)
                    argv2[myindex] = '\0';
                runCommand();
                resetCommand();
            }
            // Use spaces to delimit parts of the command
            else if (chr == ' ')
            {
                // Step through the arguments
                if (arg == 0)
                    arg = 1;
                else if (arg == 1)
                {
                    argv1[myindex] = '\0';
                    arg = 2;
                    myindex = 0;
                }
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
                    argv1[myindex] = chr;
                    myindex++;
                }
                else if (arg == 2)
                {
                    argv2[myindex] = chr;
                    myindex++;
                }
            }
        }

        // If we are using base control, run a PID calculation at the appropriate intervals

        if (to_ms_since_boot(get_absolute_time()) > nextPID)
        {
            updatePID();
            nextPID += PID_INTERVAL;
        }

        // Check to see if we have exceeded the auto-stop interval
        if ((to_ms_since_boot(get_absolute_time()) - lastMotorCommand) > AUTO_STOP_INTERVAL)
        {
            ;
            setMotorSpeeds(0, 0);
            moving = 0;
        }
    }
}
