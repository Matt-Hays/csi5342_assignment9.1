#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/* ------------------- public status codes ------------------- */
typedef enum {
    PUMP_OK = 0,
    PUMP_ERR_RATE_OUT_OF_RANGE,
    PUMP_ERR_STATE
} pump_error_t;

/* ------------------- public state machine ------------------ */
typedef enum {
    STATE_IDLE = 0,
    STATE_INFUSING,
    STATE_PAUSED,
    STATE_ALARM
} pump_state_t;

/* ------------------- public API ---------------------------- */
void        pump_init(void);

pump_state_t pump_get_state(void);
uint16_t     pump_get_rate(void); // mL / h
uint32_t     pump_get_delivered_ul(void); // µL

pump_error_t pump_set_rate(uint16_t ml_per_hr); // 1-1200 mL/h

pump_error_t pump_start(void);
pump_error_t pump_pause(void);
pump_error_t pump_resume(void);
pump_error_t pump_stop(void);

// scheduler “tick”
void pump_tick_ms(uint32_t ms);

#endif
