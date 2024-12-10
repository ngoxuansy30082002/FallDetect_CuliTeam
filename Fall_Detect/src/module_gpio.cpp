#include "module_gpio.h"
#define DEBOUNCE_DELAY 200 // Thời gian debounce (ms)

mod_gpio_data modGpioDt;
volatile unsigned long lastInterruptBtn = 0;

void IRAM_ATTR _handleInterruptBtnOledOnOff()
{
    unsigned long currentTime = millis();
    if (currentTime - lastInterruptBtn > DEBOUNCE_DELAY)
    {
        lastInterruptBtn = currentTime; // Cập nhật thời gian lần nhấn nút
        modOledDt.sleepFlag = !modOledDt.sleepFlag;
        modGpioDt.pinOledPressed = true;
    }
}

volatile unsigned long lastInterruptStopBtn = 0;

void IRAM_ATTR _handleInterruptBtnStopAlarm()
{
    unsigned long currentTime = millis();
    if (currentTime - lastInterruptStopBtn > DEBOUNCE_DELAY)
    {
        lastInterruptStopBtn = currentTime; // Cập nhật thời gian lần nhấn nút
        if (mainDt.state == FALL_DETECT_ALARM)
            modGpioDt.pinStopAlarmPressed = true;
    }
}

void MOD_GPIO_Init()
{
    pinMode(PIN_OLED_ONOFF, INPUT_PULLUP);
    pinMode(PIN_STOP_ALARM, INPUT_PULLUP);
    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_ADC, INPUT);

    attachInterrupt(PIN_OLED_ONOFF, _handleInterruptBtnOledOnOff, FALLING);
    attachInterrupt(PIN_STOP_ALARM, _handleInterruptBtnStopAlarm, FALLING);
    modGpioDt.pinOledPressed = false;
    modGpioDt.pinStopAlarmPressed = false;
}
void MOD_GPIO_Task()
{
}