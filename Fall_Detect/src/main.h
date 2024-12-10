#ifndef MOD_MAIN_H /* include guards */
#define MOD_MAIN_H

#include "define.h"

typedef enum
{
    IDLE = 0,
    VARIATION_DETECT,
    HERTBEAT_DETECT,
    FALL_DETECT_ALARM
} main_state_t;

typedef struct
{
    main_state_t state;
    unsigned long fingerCheckStartTime;
} main_data;

extern main_data mainDt;

#endif /* MOD_MAIN_H */