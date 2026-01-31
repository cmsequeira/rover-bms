// Implement the State Machine

#include <errno.h>
#include <stddef.h>

#include "bms.h" // BMS interface
#include "bms_state.h" // BMS states
#include "bms_define.h" // BMS constants

void bms_init(bms_outputs_t *outputs) 
{
    if (outputs != NULL) 
    {
        outputs->state = BMS_INIT;
        outputs->charge_enabled = false;
        outputs->discharge_enabled = false;
        outputs->fault_active = false;
        outputs->fault_flags = FAULT_NONE;
        return true;
    }
    return false;
}

void bms_run(const bms_inputs_t *inputs, bms_outputs_t *outputs) 
{
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
    if (inputs->charger_connected && (inputs->current > MAX_CHARGE_CURRENT) || 
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
        return;
    }

    // ----- State Machine -----
    switch (outputs->state) {

    case BMS_INIT:
        // Transition to STANDBY after initialization
        idle_time_ms = 0;
        outputs->state = BMS_STANDBY;
        break;

    case BMS_STANDBY:
        idle_time_ms = 0;

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

    case BMS_SLEEP:
        if (inputs->wake_request) {
            idle_time_ms = 0;
            outputs->state = BMS_STANDBY;
        }
        break;

    case BMS_CHARGING:
        outputs->charge_enabled = true;
        idle_time_ms = 0;

        if (outputs->fault_flags != FAULT_NONE) {
            outputs->state = BMS_FAULT;
            break;
        }

        if (!inputs->charger_connected) {
            outputs->state = BMS_STANDBY;
            break;
        }
        break;

    case BMS_DISCHARGING:
        outputs->discharge_enabled = true;
        idle_time_ms = 0;

        if (outputs->fault_flags != FAULT_NONE) {
            outputs->state = BMS_FAULT;
            break;
        }

        if (inputs->load_request == LOAD_MINIMAL) {
            outputs->state = BMS_STANDBY;
            break;
        }

    case BMS_FAULT:
        // Remain in FAULT until fault reset
        outputs->fault_active = true;

        if (inputs->fault_reset) {
            outputs->fault_flags = FAULT_NONE;
            outputs->fault_active = false;
            outputs->state = BMS_STANDBY;
        }
        break;

    default:
        outputs->state = BMS_INIT;
        break;
    }
}