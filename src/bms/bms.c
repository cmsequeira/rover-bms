// Implement the State Machine

#include <errno.h>
#include <stddef.h>

#include "bms.h"        // BMS interface
#include "bms_state.h"  // BMS states
#include "bms_define.h" // BMS constants

// Initialize the BMS outputs to default values
int bms_init(bms_outputs_t *outputs) 
{
    // Check for null pointer
    if (outputs == NULL) {
        return -EINVAL;
    }

    // Set initial state and defaults
    outputs->state = BMS_INIT;
    outputs->charge_enabled = false;
    outputs->discharge_enabled = false;
    outputs->fault_active = false;
    outputs->fault_flags = FAULT_NONE;

    return 0;
}

// Main BMS logic function - processes inputs and updates outputs
int bms_run(const bms_inputs_t *inputs, bms_outputs_t *outputs) 
{
    // Check for null pointers
    if (inputs == NULL || outputs == NULL) {
        return -EINVAL;
    }
    
    // Static variable to track idle time for sleep transition
    static uint32_t idle_time_ms = 0;

    // ----- Fault Detection -----
    bms_fault_t detected_fault = FAULT_NONE;
    
    // Voltage Faults
    if (inputs->voltage > MAX_VOLTAGE) {
        detected_fault |= FAULT_OVERVOLTAGE;
    }

    if (inputs->voltage < MIN_VOLTAGE) {
        detected_fault |= FAULT_UNDERVOLTAGE;
    }

    // Current Faults
    if ((inputs->charger_connected && (inputs->current > MAX_CHARGE_CURRENT)) || 
        (!inputs->charger_connected && (inputs->current > MAX_DISCHARGE_CURRENT))) {
        detected_fault |= FAULT_OVERCURRENT;
    }

    // Temperature Faults
    if (inputs->temperature > MAX_TEMPERATURE) {
        detected_fault |= FAULT_OVERTEMPERATURE;
    }

    if (inputs->temperature < MIN_TEMPERATURE) {
        detected_fault |= FAULT_UNDERTEMPERATURE;
    }

    // Check if any faults were detected
    if (detected_fault != FAULT_NONE) {
        outputs->fault_flags = detected_fault;
        outputs->fault_active = true;
        outputs->state = BMS_FAULT;
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;
        return 0;
    }

    // ----- State Machine -----

    // State transitions based on current state and inputs
    switch (outputs->state) {

    // Initial state - transition to standby
    case BMS_INIT:
        idle_time_ms = 0;
        outputs->state = BMS_STANDBY;
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;
        outputs->fault_active = false;
        outputs->fault_flags = FAULT_NONE;
        break;

    // Standby state - can transition to charging, discharging, sleep or fault
    case BMS_STANDBY:
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;
        outputs->fault_active = false;

        if (outputs->fault_flags != FAULT_NONE) {
            outputs->state = BMS_FAULT;
            break;
        }

        if (inputs->charger_connected) {
            outputs->state = BMS_CHARGING;
            break;
        }

        if (inputs->load_request == LOAD_MINIMAL) {
            idle_time_ms += inputs->delta_time_ms;
            if (idle_time_ms >= sleep_delay_ms && !inputs->wake_request) {
                outputs->state = BMS_SLEEP;
            }
        } else if (inputs->load_request != LOAD_MINIMAL) {
            outputs->state = BMS_DISCHARGING;
        }
        break;

    // Sleep state - can only transition back to standby on wake request
    case BMS_SLEEP:
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;

        if (inputs->wake_request) {
            idle_time_ms = 0;
            outputs->state = BMS_STANDBY;
        }
        break;

    // Charging state - can transition to standby or fault
    case BMS_CHARGING:
        idle_time_ms = 0;
        outputs->charge_enabled = true;
        outputs->discharge_enabled = false;

        if (outputs->fault_flags != FAULT_NONE) {
            outputs->state = BMS_FAULT;
            break;
        }

        if (!inputs->charger_connected) {
            outputs->state = BMS_STANDBY;
            break;
        }
        break;

    // Discharging state - can transition to standby or fault
    case BMS_DISCHARGING:
        idle_time_ms = 0;
        outputs->charge_enabled = false;
        outputs->discharge_enabled = true;

        if (outputs->fault_flags != FAULT_NONE) {
            outputs->state = BMS_FAULT;
            break;
        }

        if (inputs->load_request == LOAD_MINIMAL) {
            outputs->state = BMS_STANDBY;
            break;
        }

        break;

    // Fault state - can only transition back to standby on fault reset
    case BMS_FAULT:
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;
        outputs->fault_active = true;

        if (inputs->fault_reset) {
            outputs->fault_flags = FAULT_NONE;
            outputs->fault_active = false;
            outputs->state = BMS_STANDBY;
            idle_time_ms = 0;
        }
        break;

    // set defaults
    default:
        outputs->state = BMS_INIT;
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;
        outputs->fault_active = false;
        outputs->fault_flags = FAULT_NONE;
        idle_time_ms = 0;
        break;
    }

    return 0;
}