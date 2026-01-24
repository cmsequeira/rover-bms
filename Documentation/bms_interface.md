## BMS Interface

Inputs:
- (Float) Battery voltage (V)
- (Float) Battery current (A)
- (Float) Battery Temperature (°C)
- (Bool) Charger connected
- (Enum) Load request level

Outputs:
- BMS State
- (Bool) Charge enable / disable
- (Bool) Discharge enable / (Float) limit
- (Bool) Fault flags