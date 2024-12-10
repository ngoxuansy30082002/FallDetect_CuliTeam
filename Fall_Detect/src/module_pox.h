#ifndef MOD_POX_H /* include guards */
#define MOD_POX_H

#include "define.h"

#define WINDOW_SIZE 60 // 60 seconds
#define REPORTING_PERIOD_MS 200

void MOD_POX_Init();
void MOD_POX_Task();
void MOD_POX_Shutdown();
void Reset();
typedef enum
{
    POX_IDLE = 0,
    POX_MEASURING,
    POX_POWER_OFF
} POX_state_t;

typedef struct
{
    bool fingerDetected;
    float heartRate;
    float SpO2;
    float avgHeartRate;
    float avgSpO2;
    POX_state_t state;
} mod_pox_data;

extern mod_pox_data modPoxDt;

#endif /* MOD_MPU_H */