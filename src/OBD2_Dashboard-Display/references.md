
# Sensor Value Reference Guide

## 🔹 Airflow Sensor (MAF) - Units: g/s (grams per second)
> ⚠️ **Estimated values based on typical naturally aspirated and turbocharged engines.**  
> ⚠️ **Always refer to the vehicle's service manual or manufacturer specifications.**  

| Engine Type         | Idle (g/s) | Light Load (g/s) | Full Throttle (g/s) |
|---------------------|-----------|------------------|--------------------|
| Small 4-Cyl (1.0L-2.0L) | 2-5       | 10-50            | 100-200        |
| V6 (2.5L-3.5L)     | 3-7       | 15-80            | 150-300             |
| V8 (4.0L-6.0L)     | 5-10      | 20-100           | 200-500             |
| Turbo 4-Cyl        | 3-8       | 20-100           | 250-400             |
| Turbo V6/V8        | 5-12      | 30-150           | 300-600             |

✅ **High airflow at idle** = Possible vacuum leak or bad sensor.  
✅ **Low airflow at high RPM** = Intake restriction or failing turbo.  


## 🔹 Oil Pressure Sensor - Units: PSI (or Bar conversion)
> ⚠️ **Estimated values based on standard automotive engines.**  
> ⚠️ **Always check the vehicle’s service manual or consult with a tuner for modified engines.**  

| Engine Type        | Cold Start (PSI) | Idle (PSI) | Running (PSI) | Max (PSI) |
|--------------------|----------------|------------|--------------|----------|
| Small 4-Cyl (1.0L-2.0L) | 50-70 | 10-20 | 30-50 | 60-80  |
| V6 (2.5L-3.5L)    | 60-80 | 15-25 | 40-60 | 80-100  |
| V8 (4.0L-6.0L)    | 70-90 | 20-30 | 50-70 | 100-120  |
| Turbocharged Engines | 70-100 | 20-35 | 50-80 | 110-130 |
| Race Engines (Dry Sump) | 80-120 | 30-50 | 60-100 | 150+ |

✅ **Too low** = Possible oil pump failure, worn bearings.  
✅ **Too high** = Blocked oil passages or faulty pressure relief valve.  


## Flash settings for the JC3248W53C ESP32S3 Display
>  https://spotpear.com/wiki/ESP32-S3-3.5-inch-LCD-Captive-TouchScreen-Display-480x320-Tablet-MP3-Video-Weather-Clock.html


