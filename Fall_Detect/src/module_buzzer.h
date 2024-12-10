#ifndef MOD_BUZZER_H /* include guards */
#define MOD_BUZZER_H

#define dimming_time 1 // minutes
#include "define.h"

typedef struct
{
    int duty;
    bool isRinging;
} mod_buzzer_data;

void MOD_BUZZER_Init();
void MOD_BUZZER_Task();
void turn_off_buzzer();
void startRinging();

extern mod_buzzer_data modBuzzerDt;

#endif /* MOD_BUZZER_H */