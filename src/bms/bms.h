// Define the inputs and outputs of the BMS based on interface.md

#ifndef BMS_H
#define BMS_H

#include <stdbool.h>
#include <stdint.h>

#include "bms_state.h"  // Includes the BMS states
#include "bms_define.h" // Includes BMS constants

// Load request levels
typedef enum {
    LOAD_MINIMAL,                  // Sleep mode
    LOAD_LOW,                      // Standby mode
    LOAD_MEDIUM,                   // Regular operational mode
    LOAD_HIGH                      // High power mode
} load_request_t;

// INPUTS to the BMS
typedef struct {
    float voltage;                 // volts
    float current;                 // amps
    float temperature;             // degrees Celsius
    bool charger_connected;        // True if external power source is available
    bool wake_request;             // Request to wake from sleep
    bool fault_reset;              // Latch for system recovery
    uint32_t delta_time_ms;        // Time since last update
    load_request_t load_request;   // Requested load level
} bms_inputs_t;

// Fault codes
typedef uint32_t bms_fault_t;

// OUTPUTS from the BMS
typedef struct {
    bms_state_t state;             // Current BMS state -> from bms_state.h
    bool charge_enabled;           // Allow charging
    bool discharge_enabled;        // Allow discharging
    bool fault_active;             // True if in fault condition
    bms_fault_t fault_flags;       // Fault bitmask
} bms_outputs_t;

// Function to update BMS state
int bms_init(bms_outputs_t *outputs);
int bms_run(const bms_inputs_t *inputs, bms_outputs_t *outputs);

#endif // BMS_H