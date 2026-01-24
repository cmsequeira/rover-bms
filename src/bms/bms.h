// Define the inputs and outputs of the BMS based on interface.md

#ifndef BMS_H
#define BMS_H

#include <stdbool.h>
#include "bms_state.h"             // Includes the BMS states

typedef enum {
    LOAD_MINIMAL,                  // Sleep mode
    LOAD_LOW,                      // Idle mode
    LOAD_MEDIUM,                   // Regular operational mode
    LOAD_HIGH                      // High power mode
} load_request_t;

// Inputs to the BMS
typedef struct {
    float voltage;                 // volts
    float current;                 // amps
    float temperature;             // degrees Celsius
    bool charger_connected;        // True if external power source is available
    load_request_t load_request;   // Requested load level
} bms_inputs_t;

// Outputs from the BMS
typedef struct {
    bms_state_t state;             // Current BMS state
    bool charge_enabled;           // Allow charging
    bool discharge_enabled;        // Allow discharging
    // float discharge_limit; 
    bool fault_flag;               // Fault detected
} bms_outputs_t;

// Function to update BMS state
void bms_update(const bms_inputs_t* inputs, bms_outputs_t* outputs);

#endif // BMS_H