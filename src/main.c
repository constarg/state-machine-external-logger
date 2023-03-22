#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "pico/sync.h"
#include "hardware/clocks.h"
#include "hardware/structs/systick.h"

#include <inttypes.h>

#define GPIOS_LEN 11 // the number of gpios.


static uint32_t signal_buff[200] = {0}; // The queue of signals.
static volatile int8_t signal_index = -1; // Where to store the next signal.

static uint32_t prev_signal = 0;

/**
 * This function print the value of each signal.
 * 
 * @param src The signal buffer to print.
 */
static inline void print_signals(uint32_t src, uint32_t timestamp)
{
    printf("%d ", timestamp);
    for (int sig = 0; sig < GPIOS_LEN; sig++) {
        printf("%d", (src >> sig) & (0x1));
    }
    printf("\n");
}

// Interrupt service routine for the gpios.
static void state_logger_gpio_isr(uint gpio, uint32_t event_mask) 
{
    uint8_t intr_state = 0;
    signal_index += 1;

    intr_state = save_and_disable_interrupts();
    systick_hw->cvr = 0x0;
    systick_hw->csr = 0x5;
    systick_hw->rvr = 0x00ffffff;
    
    uint32_t start = systick_hw->cvr; // start

    // Get the values of all 11 gpios. Ignore the others using the mask 0x7FF.
    signal_buff[signal_index] = gpio_get_all();
    uint32_t end = systick_hw->cvr;

    printf("%u\n", (start - end) * 8);
    restore_interrupts(intr_state);
}

// Initialize all the gpios to be used.
static void state_logger_init_gpios()
{
    gpio_init(0);
    gpio_set_dir(0, GPIO_IN);
    gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
                                       true, &state_logger_gpio_isr);

    for (int gpio = 1; gpio < GPIOS_LEN; gpio++) {
        // initilize current gpio
        gpio_init(gpio);
        // set current gpio to input mode.
        gpio_set_dir(gpio, GPIO_IN);
        // enable interrups for each bpio.
        gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }
}


int main(void)
{
    uint32_t curr_signal = 0;

    stdio_init_all();
    state_logger_init_gpios();

    while (true) {
        if (signal_index > -1) {
            curr_signal = signal_buff[signal_index];
            signal_index -= 1;
            if (curr_signal == prev_signal) continue;
            prev_signal = curr_signal;
            print_signals(curr_signal, time_us_32());
        }
    }
}
