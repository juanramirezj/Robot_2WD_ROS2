/***************************************************************
   Motor driver function definitions - by James Nugen
   *************************************************************/


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

uint32_t pwm_set_freq_duty(uint slice_num, uint chan, uint32_t f, int d);
uint32_t pwm_get_wrap(uint slice_num);
void pwm_set_duty(uint slice_num, uint chan, int d);
void BiMotorInit(BiMotor *m, uint gpioForward, uint gpioBackward, uint freq);
void BiMotorspeed(BiMotor *m, int s, bool forward);
void BiMotorOn(BiMotor *m);
void BiMotorOff(BiMotor *m);
void initMotorController();
void setMotorSpeed(int i, int spd);
void setMotorSpeeds(int leftSpeed, int rightSpeed);