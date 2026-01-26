// Define the inputs and outputs of the BMS based on interface.md

#ifndef BMS_H
#define BMS_H

#include <stdbool.h>
#include <stdint.h>
#include "bms_state.h"             // Includes the BMS states

// Load request levels
typedef enum {
    LOAD_NONE,                     // No load
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
    load_request_t load_request;   // Requested load level
} bms_inputs_t;

// Fault codes
typedef uint32_t bms_fault_t;

#define FAULT_NONE             0u          // No fault
#define FAULT_OVERVOLTAGE      (1u << 0)   // Voltage too high
#define FAULT_UNDERVOLTAGE     (1u << 1)   // Voltage too low
#define FAULT_OVERCURRENT      (1u << 2)   // Current too high
#define FAULT_OVERTEMPERATURE  (1u << 3)   // Temperature too high
#define FAULT_UNDERTEMPERATURE (1u << 4)   // Temperature too low

// OUTPUTS from the BMS
typedef struct {
    bms_state_t state;             // Current BMS state -> from bms_state.h
    bool charge_enabled;           // Allow charging
    bool discharge_enabled;        // Allow discharging
    bool fault_active;             // True if in fault condition
    bms_fault_t fault_flag;        // Specific fault code
} bms_outputs_t;

// Function to update BMS state
void bms_init(bms_outputs_t *outputs);
void bms_update(const bms_inputs_t *inputs, bms_outputs_t *outputs);

#endif // BMS_H