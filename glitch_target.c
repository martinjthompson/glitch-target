#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/watchdog.h"

volatile int running = 1;

static const uint32_t WATCHDOG_TIMEOUT_MS = 200;
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
    uint32_t current_duration = 0;
    pwm_set_enabled(slice_num, 0);
    (void)pwm_set_freq_duty(slice_num, chan, f, 50);
    pwm_set_enabled(slice_num, 1);
    if (duration_ms > WATCHDOG_TIMEOUT_MS/2)
    {
        for (int i=0;i<duration_ms/50;++i)
        {            
            watchdog_update();
            sleep_ms(50);
            current_duration += 50;
        }
        watchdog_update();
        sleep_ms(duration_ms - current_duration);
    }
    else
    {
        sleep_ms(duration_ms);
    }
    pwm_set_enabled(slice_num, 0);
}

/* Simple public domain implementation of the standard CRC32 checksum.
 * Outputs the checksum for each file given as a command line argument.
 * Invalid file names and files that cause errors are silently skipped.
 * The program reads from stdin if it is called with no arguments. 
 * http://home.thep.lu.se/~bjorn/crc/
 * */

uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

void crc32(const void *data, size_t n_bytes, uint32_t* crc) {
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
}

/* Pretend the application is 128KB of all zeros, calculate the CRC32 of it and compare with the expected result...*/
#define APPLICATION_CODE_LENGTH (1024*1UL)
const uint8_t APP[APPLICATION_CODE_LENGTH] = {0,};
const uint32_t EXPECTED_CRC32 = 0xFFFFFFFF;
bool check_application(void)
{
    uint32_t crc = 0xFFFFFFFF;
    crc32(APP, APPLICATION_CODE_LENGTH, &crc);
    uint32_t calculated_crc32 = crc;
    if (calculated_crc32 != EXPECTED_CRC32)
    {
        printf("CRCs do not match: 0x%08x != 0x%08x", calculated_crc32, EXPECTED_CRC32);
        return false;
    }
    return true;
}

int main (void)
{
    const int BUZZER_PIN=9;
    const int LED_PIN=25;
    const bool CONTINUOUS_PINGS=false;
    const bool TEST_MODE=false; // set to TRUE to force it through the various modes
    bool application_good;
    uint32_t i;
    stdio_init_all();
    gpio_init(BUZZER_PIN);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    unsigned slice=pwm_gpio_to_slice_num (BUZZER_PIN); 
    unsigned channel=pwm_gpio_to_channel (BUZZER_PIN);

    if (watchdog_caused_reboot()) 
    {
        printf("\n\nRebooted by Watchdog\n\n");
        pwm_chirp(slice, channel, 800, 100);
    }
    else
    {
        puts("\n\nStartup\n");
        pwm_chirp(slice, channel, 400, 100);
    }
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    watchdog_enable(WATCHDOG_TIMEOUT_MS, 1);

    while (running)
    {
        puts("Checking 'Application'");
        application_good = true;
        while (application_good)
        {
            watchdog_update();
            gpio_put(LED_PIN, 0);
            ++i;
            if (i%1000 == 0)
            {
                putchar('.');
            }
            if (CONTINUOUS_PINGS)
            {
                pwm_chirp(slice, channel, 400, 10);
            }
            application_good = check_application();
            gpio_put(LED_PIN, 1);
            if (TEST_MODE && ((i%10) == 0))
                break;
        }
        puts("\nFall-back to 'Insecure Bootloader'\n");
        pwm_chirp(slice, channel, 1600, 400);
        if (TEST_MODE && ((i%30) == 0))
            break;
    }
    puts("\nFallen past the 'Insecure Bootloader'...");
    for (int i=0;i<10;++i)
    {
        pwm_chirp(slice, channel, 200, 100);
        sleep_ms(200);
    }
    putchar('\n');
}