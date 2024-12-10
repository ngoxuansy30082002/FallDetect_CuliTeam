#ifndef MOD_GPIO_H /* include guards */
#define MOD_GPIO_H

#include "define.h"

void MOD_GPIO_Init();
void MOD_GPIO_Task();

typedef struct
{
    bool pinOledPressed;
    bool pinStopAlarmPressed;
} mod_gpio_data;

extern mod_gpio_data modGpioDt;

#endif /* MOD_GPIO_H */