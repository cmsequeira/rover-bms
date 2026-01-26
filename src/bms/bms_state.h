// Define the states of the BMS

#ifndef BMS_STATE_H
#define BMS_STATE_H

typedef enum {
    BMS_INIT,         // Initializing
    BMS_STANDBY,      // Rover is on but not active
    BMS_CHARGING,     // Rover being charged
    BMS_DISCHARGING,  // Rover consuming power
    BMS_FAULT,        // Safety limit exceeded
    BMS_SLEEP         // Low power mode
} bms_state_t;

#endif // BMS_STATE_H