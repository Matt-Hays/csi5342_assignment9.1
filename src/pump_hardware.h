#ifndef PUMP_HARDWARE_H
#define PUMP_HARDWARE_H
#include <stdint.h>

void hardware_motor_on(void);
void hardware_motor_off(void);
void hardware_set_rate(uint16_t ml_per_hr);
void hardware_trigger_alarm(void);

#endif