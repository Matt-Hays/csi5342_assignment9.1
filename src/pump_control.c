#include "pump_control.h"
#include "pump_hardware.h"

#define MIN_RATE_ML_HR 1
#define MAX_RATE_ML_HR 1200

static pump_state_t state = STATE_IDLE;
static uint16_t rate_ml_hr = 0;
static uint32_t delivered_ul = 0;

// Integer conversion of volume
static inline uint32_t ul_from_ms(uint16_t ml_per_hr, uint32_t ms)
{
    return (uint32_t) (( (uint64_t) ml_per_hr * ms * 1000ULL) / 3600000ULL);
}

void pump_init(void)
{
    state = STATE_IDLE;
    rate_ml_hr = 0;
    delivered_ul = 0;
    hardware_motor_off();
}

pump_state_t pump_get_state(void) { return state; }
uint16_t pump_get_rate(void) { return rate_ml_hr; }
uint32_t pump_get_delivered_ul(void) { return delivered_ul; }

pump_error_t pump_set_rate(uint16_t ml_per_hr)
{
    if (ml_per_hr < MIN_RATE_ML_HR || ml_per_hr > MAX_RATE_ML_HR)
        return PUMP_ERR_RATE_OUT_OF_RANGE;

    rate_ml_hr = ml_per_hr;

    if (state == STATE_INFUSING)
        hardware_set_rate(rate_ml_hr);

    return PUMP_OK;
}

pump_error_t pump_start(void)
{
    if (state != STATE_IDLE || rate_ml_hr == 0)
        return PUMP_ERR_STATE;

    state = STATE_INFUSING;
    delivered_ul = 0;
    hardware_set_rate(rate_ml_hr);
    hardware_motor_on();
    return PUMP_OK;
}

pump_error_t pump_pause(void)
{
    if (state != STATE_INFUSING)
        return PUMP_ERR_STATE;

    state = STATE_PAUSED;
    hardware_motor_off();
    return PUMP_OK;
}

pump_error_t pump_resume(void)
{
    if (state != STATE_PAUSED)
        return PUMP_ERR_STATE;

    state = STATE_INFUSING;
    hardware_motor_on();
    return PUMP_OK;
}

pump_error_t pump_stop(void)
{
    if (state == STATE_IDLE)
        return PUMP_ERR_STATE;
    state = STATE_IDLE;
    hardware_motor_off();
    return PUMP_OK;
}

void pump_tick_ms(uint32_t ms)
{
    if (state == STATE_INFUSING && rate_ml_hr)
        delivered_ul += ul_from_ms(rate_ml_hr, ms);
}