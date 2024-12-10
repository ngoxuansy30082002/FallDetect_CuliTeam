#ifndef MOD_OLED_H /* include guards */
#define MOD_OLED_H
#include "define.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

typedef enum
{
    OLED_IDLE = 0,
    OLED_POWER_OFF,
    OLED_DISPLAY_WAIT_FINGER,
    OLED_DISPLAY_MEASURING,
} OLED_display_state_t;

typedef struct
{
    bool sleepFlag;
    OLED_display_state_t state;
} mod_oled_data;

void MOD_OLED_Init();
void MOD_OLED_Task();
void MOD_OLED_EnterState(OLED_display_state_t state);
void MOD_OLED_Shutdown();

extern mod_oled_data modOledDt;

#endif /* MOD_OLED_H */