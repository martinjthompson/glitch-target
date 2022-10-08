#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

volatile int running = 1;

// From here: https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=2
uint32_t pwm_set_freq_duty(uint slice_num,
       uint chan,uint32_t f, int d)
{
    uint32_t clock = 125000000;
    uint32_t divider16 = clock / f / 4096 + 
                            (clock % (f * 4096) != 0);
    if (divider16 / 16 == 0)
    divider16 = 16;
    uint32_t wrap = clock * 16 / divider16 / f - 1;
    pwm_set_clkdiv_int_frac(slice_num, divider16/16,
                                        divider16 & 0xF);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, chan, wrap * d / 100);
    return wrap;
}

void pwm_chirp(uint slice_num, uint chan, uint32_t f, uint32_t duration_ms)
{
    pwm_set_enabled(slice_num, 0);
    (void)pwm_set_freq_duty(slice_num, chan, f, 50);
    pwm_set_enabled(slice_num, 1);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, 0);
}


int main (void)
{
    volatile unsigned i = 0;
    stdio_init_all();
    // GP9 is the buzzer (PWMB4)
    static const int BUZZER_PIN=9;
    gpio_init(BUZZER_PIN);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    unsigned slice=pwm_gpio_to_slice_num (BUZZER_PIN); 
    unsigned channel=pwm_gpio_to_channel (BUZZER_PIN);
    uint16_t wrap = 20000;

    pwm_chirp(slice, channel, 400, 10);
    putchar('M');
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    pwm_set_enabled(slice, 0);

    while (running)
    {
        putchar('S');
        while (running)
        {
            ++i;
            #if 0
            gpio_put(25, 1);
            sleep_ms(100);
            gpio_put(25, 0);
            sleep_ms(100);
            putchar('.');
            pwm_chirp(slice, channel, 800, 10);
            #endif
#ifdef TEST
            if ((i%10) == 0)
                break;
#endif
        }
        putchar('!');
        pwm_chirp(slice, channel, 1600, 500);
#ifdef TEST
        if ((i%30) == 0)
            break;
#endif
    }
    putchar('\n');
    for (int i=0;i<10;++i)
    {
        pwm_chirp(slice, channel, 200, 100);
        putchar('E');
        sleep_ms(200);
    }
    putchar('\n');
}