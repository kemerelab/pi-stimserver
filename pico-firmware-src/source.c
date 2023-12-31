#include "source.h"
#include <stdio.h>
#include "pico/stdlib.h"

// Notes - https://forums.raspberrypi.com/viewtopic.php?t=300754  

int main() {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint IN_PIN = 22; // GPIO 22
    const uint OUT_PIN = 0; // GPIO 0
    const uint MIRROR_PIN = 1; // GPIO 1
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(OUT_PIN);
    gpio_set_dir(OUT_PIN, GPIO_OUT);
    gpio_init(MIRROR_PIN);
    gpio_set_dir(MIRROR_PIN, GPIO_OUT);
    gpio_init(IN_PIN);
    gpio_set_dir(IN_PIN, GPIO_IN);

    uint state = 0;

    gpio_put(LED_PIN, 1);

    while (true) {
        if ((gpio_get(IN_PIN) == 1) && (state == 0)) {
            gpio_put(MIRROR_PIN, 1);
            gpio_put(OUT_PIN, 1);
            sleep_us(100);
            gpio_put(OUT_PIN, 0);
            sleep_us(6);
            gpio_put(OUT_PIN, 1);
            sleep_us(100);
            gpio_put(OUT_PIN, 0); 
            state = 1;
        }
        else if (gpio_get(IN_PIN) == 0) {
            gpio_put(OUT_PIN, 0);
            gpio_put(MIRROR_PIN, 0);
            state = 0;
        }
    }
#endif
}