/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "LCD.h"

lcd mylcd;
bool setupMode = true;

int hours = 0;
int minutes = 0;
int seconds = 0;
    

InterruptIn hoursInterrupt(PA_1);
InterruptIn minutesInterrupt(PA_6);
InterruptIn secondsInterrupt(PA_10);
InterruptIn switchMode(PB_4);

void changeHours() {
    if (hours >= 23) {
        hours = 0;
    } else {
        hours++;
    }
}

void changeMinutes() {
    if (minutes >= 59) {
        minutes = 0;
    } else {
        minutes++;
    }
}

void changeSeconds() {
    if (seconds >= 59) {
        seconds = 0;
    } else {
        seconds++;
    }
}

void toggleSwitch() {
    setupMode = !setupMode;

    if (setupMode) {
        TIM6 -> CR1 = 0;

        hoursInterrupt.enable_irq();
        minutesInterrupt.enable_irq();
        secondsInterrupt.enable_irq();
    } else {
        TIM6 -> CNT = 0;
        TIM6 -> CR1 = 1;

        hoursInterrupt.disable_irq();
        minutesInterrupt.disable_irq();
        secondsInterrupt.disable_irq();
    }
}

void initTimer() {
    RCC -> APB1ENR |= 0b10000;
    TIM6 -> PSC = 31999;
    TIM6 -> ARR = 1000;
    TIM6 -> CNT = 0;
    TIM6 -> SR = 0;
    TIM6 -> CR1 = 0;
}

void initInterrupts() {
    hoursInterrupt.mode(PullDown);
    minutesInterrupt.mode(PullDown);
    secondsInterrupt.mode(PullDown);
    switchMode.mode(PullDown);

    hoursInterrupt.rise(&changeHours);
    minutesInterrupt.rise(&changeMinutes);
    secondsInterrupt.rise(&changeSeconds);

    switchMode.rise(&toggleSwitch);

    hoursInterrupt.enable_irq();
    minutesInterrupt.enable_irq();
    secondsInterrupt.enable_irq();

    switchMode.enable_irq();
}

int main()
{
    initTimer();
    initInterrupts();

    mylcd.clear();
    mylcd.cursorpos(0);

    while (true) {

        if (!setupMode) {
            if (TIM6 -> SR == 1) {
                TIM6 -> SR = 0;
                seconds++;
            }

            if (seconds >= 60) {
                seconds = 0;
                minutes++;

                if (minutes >= 60) {
                    minutes = 0;
                    hours++;

                    if (hours >= 24) {
                        hours = 0;
                    }
                }
            }
        }
        
        mylcd.printf("%02d:%02d:%02d", hours, minutes, seconds);
        mylcd.cursorpos(0);
    }
}
