#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

//#include <StandardCplusplus.h>
#include <vector>

#define BLINK_GPIO (gpio_num_t)CONFIG_BLINK_GPIO

#ifndef LED_BUILTIN
#define LED_BUILTIN 4
#endif

struct Pack {
    int ledNum;
    int speed;
};

void blink_task(void *pvParameter)
{

    auto pack = static_cast<Pack*>(pvParameter);

    auto ledGPIO = gpio_num_t(pack->ledNum);
    auto speed = pack->speed;


    gpio_pad_select_gpio(ledGPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(ledGPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(ledGPIO, LOW);
        vTaskDelay(speed / portTICK_PERIOD_MS);
        /* Blink on (output high) */

        gpio_set_level(ledGPIO, HIGH);
        vTaskDelay(speed / portTICK_PERIOD_MS);
    }
}



#define PIN_STEP 17
#define PIN_DIR 16

// количество шагов на 1 оборот
#define STEP_ROUND 210

// скорость двигателя
#define SPEED 20

#include <gyver_stepper/GyverStepper2.h>
GStepper2<STEPPER2WIRE> stepper(2048, 2, 3);

void setup() {
    Serial.begin(115200);

    auto packOne = new Pack();
    packOne->ledNum = 38;
    packOne->speed = 500;

    auto packTwo = new Pack();
    packTwo->ledNum = 40;
    packTwo->speed = 2000;


    xTaskCreate(&blink_task, "blink_task_first", configMINIMAL_STACK_SIZE, packOne,  5, NULL);
    xTaskCreate(&blink_task, "blink_task_second", configMINIMAL_STACK_SIZE, packTwo, 5, NULL);

    // режим для STEP и DIR как OUTPUT
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    
    // pinMode(40, OUTPUT);
    // pinMode(38, OUTPUT);

    // начальные значения
    digitalWrite(PIN_STEP, 1);
    digitalWrite(PIN_DIR, 0);
}


void loop() {
    // направление вращения
    digitalWrite(PIN_DIR, HIGH);

    // digitalWrite(40, HIGH);
    // digitalWrite(38, HIGH);

    // сделать 1 оборот
    for(int j = 0; j < STEP_ROUND; j++) {
        digitalWrite(PIN_STEP, HIGH);
        delay(SPEED);
        digitalWrite(PIN_STEP, LOW);
        delay(SPEED);
    }
    // изменить направление вращения
    digitalWrite(PIN_DIR, LOW);
    // сделать 1 оборот
    for(int j = 0; j < STEP_ROUND; j++) {
        digitalWrite(PIN_STEP, HIGH);
        delay(SPEED);
        digitalWrite(PIN_STEP, LOW);
        delay(SPEED);
    }
    delay(1000);
}