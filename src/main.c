#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "pico/sync.h"
#include "hardware/clocks.h"
#include "hardware/structs/systick.h"

#include <inttypes.h>

#define GPIOS_LEN 11 // the number of gpios.

typedef uint16_t signal_buff; // The buffer in which we store the signals.

static signal_buff g_prev_signals = 0; // The previous signals.

/**
 * This function read the value of all GPIOS_LEN 
 * gpios and store each bit in the dst.
 * 
 * @param dst Where the bits of all GPIOS_LEN gpios will be saved.
 */
static inline void read_signals(signal_buff *dst)
{
    for (int sig = 0; sig < GPIOS_LEN; sig++) {
        *dst |= gpio_get(sig) << sig;
    }
}

/**
 * This function print the value of each signal.
 * 
 * @param src The signal buffer to print.
 */
static inline void print_signals(signal_buff src, uint32_t timestamp)
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
    signal_buff signals = 0;

    // read signals.
    read_signals(&signals);

    uint8_t intr_state = 0;
    intr_state = save_and_disable_interrupts();

/*    systick_hw->cvr = 0x0;
    systick_hw->rvr = 0x00ffffff;
    systick_hw->csr = 0x5;*/
//    uint32_t start = systick_hw->cvr; // start

    if (g_prev_signals == signals) {
        restore_interrupts(intr_state);
        return;
    } else {
        g_prev_signals = signals;
//        uint32_t end = systick_hw->cvr;
//        printf("Nanosecods: %d\n", (start - end) * 8);
        restore_interrupts(intr_state);
        print_signals(signals, time_us_32());
    }
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
    stdio_init_all();
    state_logger_init_gpios();

    while (true);
}
