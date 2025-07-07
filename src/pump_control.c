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
    return (uint32_t)(((uint64_t)ml_per_hr * ms * 1000ULL) / 3600000ULL);
}

/**
 * @brief Brings the pump online, sets its state to IDLE and ensures the motor is off.
 *
 */
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

/**
 * @brief Adjust the delivery rate of the pump in ml/hr.
 *
 * @param ml_per_hr The desired volume delivery amount.
 * @return pump_error_t The pump status on completion.
 */
pump_error_t pump_set_rate(uint16_t ml_per_hr)
{
    if (ml_per_hr < MIN_RATE_ML_HR || ml_per_hr > MAX_RATE_ML_HR)
        return PUMP_ERR_RATE_OUT_OF_RANGE;

    rate_ml_hr = ml_per_hr;

    if (state == STATE_INFUSING)
        hardware_set_rate(rate_ml_hr);

    return PUMP_OK;
}

/**
 * @brief Set the pump status to INFUSING, set the desired volume delivery rate, and turn the motor on.
 *
 * @return pump_error_t The pump's status on completion.
 */
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

/**
 * @brief Set the pump's status to PAUSED and turn off the motor.
 *
 * @return pump_error_t
 */
pump_error_t pump_pause(void)
{
    if (state != STATE_INFUSING)
        return PUMP_ERR_STATE;

    state = STATE_PAUSED;
    hardware_motor_off();
    return PUMP_OK;
}

/**
 * @brief Set the pump status to INFUSING from PAUSED and turn the motor on.
 *
 * @return pump_error_t The pump status.
 */
pump_error_t pump_resume(void)
{
    if (state != STATE_PAUSED)
        return PUMP_ERR_STATE;

    state = STATE_INFUSING;
    hardware_motor_on();
    return PUMP_OK;
}

/**
 * @brief Set the pump's status to STOPPED and turn the motor off.
 *
 * @return pump_error_t The pump's status.
 */
pump_error_t pump_stop(void)
{
    if (state == STATE_IDLE)
        return PUMP_ERR_STATE;
    state = STATE_IDLE;
    hardware_motor_off();
    return PUMP_OK;
}

/**
 * @brief Allows for a "tick" fuction to be based on some external, centralized clock to control scheduling.
 *
 * @param ms The tick rate (time passed since last tick).
 */
void pump_tick_ms(uint32_t ms)
{
    if (state == STATE_INFUSING && rate_ml_hr)
        delivered_ul += ul_from_ms(rate_ml_hr, ms);
}