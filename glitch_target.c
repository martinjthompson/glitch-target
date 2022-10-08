#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

volatile int running = 1;
int main (void)
{
    unsigned i = 0;
    stdio_init_all();
    // GP9 is the buzzer (PWMB4)
    static const int BUZZER_PIN=9;
    gpio_init(BUZZER_PIN);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    unsigned slice=pwm_gpio_to_slice_num (BUZZER_PIN); 
    unsigned channel=pwm_gpio_to_channel (BUZZER_PIN);
    


    putchar('M');
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    while (running)
    {
        putchar('S');
        while (running)
        {
            ++i;
            gpio_put(25, 1);
            sleep_ms(100);
            gpio_put(25, 0);
            sleep_ms(100);
            putchar('.');
            i = 0;
        }
        putchar('!');
        sleep_ms(500);
    }
    putchar('\n');
    putchar('E');
    putchar('E');
    putchar('E');
    putchar('E');
    putchar('E');
    putchar('E');
    putchar('E');
    putchar('\n');
}