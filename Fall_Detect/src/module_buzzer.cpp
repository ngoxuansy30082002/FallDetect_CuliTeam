#include "define.h"

mod_buzzer_data modBuzzerDt;

const int pwmChannel = PIN_BUZZER; // PWM channel (0-15)
const int pwmFrequency = 5000;      // Frequency in Hz (adjustable, typically 5000)
const int pwmResolution = 8;        // Resolution (8-bit: 0-255)
int delayTime = 1;
unsigned long previous = 0;

void MOD_BUZZER_Init()
{
    pinMode(PIN_BUZZER, OUTPUT);
    ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
    ledcAttachPin(PIN_BUZZER, pwmChannel);
}

void MOD_BUZZER_Task()
{
    static bool isIncrease = true;

    if (modBuzzerDt.isRinging == false)
        return;

    if (modBuzzerDt.duty == 255)
        isIncrease = false;
    else if (modBuzzerDt.duty == 0)
        isIncrease = true;

    unsigned long current = millis();
    if (current - previous >= delayTime)
    {
        previous = current;
        if (isIncrease)
            modBuzzerDt.duty++;
        else
            modBuzzerDt.duty--;
    }
    ledcWrite(PIN_BUZZER, modBuzzerDt.duty);
}

void turn_off_buzzer()
{
    ledcWrite(PIN_BUZZER, 0);
    modBuzzerDt.isRinging = false;
}
void startRinging()
{
    modBuzzerDt.isRinging = true;
    modBuzzerDt.duty = 0;
}
