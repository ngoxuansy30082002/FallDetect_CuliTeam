#ifndef MOD_MPU_H /* include guards */
#define MOD_MPU_H

#include "define.h"
#define BUFFER_SIZE 60

void MOD_MPU_Init();
void MOD_MPU_Task();

typedef enum
{
    MPU_IDLE = 0,
    MPU_MEASURING,
} MPU_state_t;

typedef struct
{
    float accelAvg;
    float gyroAvg;
    MPU_state_t state;
    float accelVar;
    float gyroVar;
} mod_mpu_data;

extern mod_mpu_data modMpuDt;

#endif /* MOD_MPU_H */