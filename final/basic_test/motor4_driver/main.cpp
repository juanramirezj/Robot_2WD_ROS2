/*****
 * Motor Driver using DRV8833 Motor Driver Circuit
 *****/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "hardware/pio.h"
#include "hardware/timer.h"
#include "quadrature_encoder.pio.h"

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





int main()
{


    int new_value1, delta1, old_value1 = 0;
    int new_value2, delta2, old_value2 = 0;

    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB1 = 18;   //20 and 26  26 is not working  20 is ok
    const uint PIN_AB2 = 20;
    
 
    stdio_init_all();


    const uint sm1 = 0;
    const uint sm2 = 1;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    // using states machine 0 and 1 from pio0
    pio_add_program(pio0, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio0, sm1, PIN_AB1, 0); 
    quadrature_encoder_program_init(pio0, sm2, PIN_AB2, 0); 


  
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
    while (1) {
        // note: thanks to two's complement arithmetic delta will always
        // be correct even when new_value wraps around MAXINT / MININT
        new_value1 = quadrature_encoder_get_count(pio0, sm1);
        delta1 = new_value1 - old_value1;
        old_value1 = new_value1;

        new_value2 = quadrature_encoder_get_count(pio0, sm2);
        delta2 = new_value2 - old_value2;
        old_value2 = new_value2;

        printf("position1 %8d, delta1 %6d  position2 %8d, delta2 %6d  \n", new_value1, delta1,new_value2, delta2);
        sleep_ms(100);
    }

    return 0;
}