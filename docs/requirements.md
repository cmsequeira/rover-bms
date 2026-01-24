# Rover BMS Design Parameters

Using the publically available information from NASA's perseverance Mars Rover, this project will use the following design parameters for the battery.

Number of Batteries: 2 Parallel Strings (8s2p architecture)*
Battery Chemistry: Lithium Ion (NMC)
Nominal Voltage: 28.8 V
Maximum Voltage: 32.8 V
Minimum Voltage: 24.0 V
Capacity: 43 Ah per string
Total Energy: 2408 Wh
Maximum Charge Current: 10 A (assumption)
Maximum Discharge Current: 22 A, 30 A pulse
Operating Temperature Range: -20°C to +30°C

*Each string can be charged independently, discharged together

NOTE: Li-ion battery cell range is 3.0-4.2V, therefore 8 cells are used.

References: 
https://www.nasa.gov/wp-content/uploads/2024/01/battery-system-design-testing-and-operation-for-the-mars-perseverance-rover-and-ingenuity-helicopter.pdf?emrc=65f7d3590b4dd (Page 5)

https://www.jpl.nasa.gov/news/press_kits/mars_2020/landing/mission/spacecraft/power/

https://saemsen.com/perseverance-rover-requires-battery-electrical-power/

https://batteryskills.com/lithium-battery-voltage-chart/