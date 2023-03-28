#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
// GPIO handler assembly.
#include "gpio_handler.pio.h"


#define GPIOS_LEN 11 // The number of used GPIOS.

static inline void print_signals(uint32_t src, uint32_t timestamp)
{
    printf("%d ", timestamp);
    for (int sig = 0; sig < GPIOS_LEN; sig++) {
        printf("%d", (src >> sig) & (0x1));
    }
    printf("\n");
}

int main(void)
{
    PIO pio = pio0; // The PIO instance/block to use.
    uint offset = 0; // The offset to the instruction memory of the state machine.
    uint sm = 0; // The state machine to be used from the pio instance.
    uint32_t curr_signals = 0; // The current values of the GPIOS.
    uint32_t prev_signals = 0; // The previous values of the GPIOS.

    stdio_init_all();
    offset = pio_add_program(pio, &gpio_handler_program);
    gpio_handler_program_init(pio, sm, offset, 0);
    pio_sm_set_enabled(pio, sm, true);

    while (true) {
        // 0x7FF is used to ignore the bits that is not from the first 11 gpios.
        curr_signals = (pio_sm_get_blocking(pio, sm) >> 21); // block until a new value.
        if (curr_signals != prev_signals) {
            print_signals(curr_signals, time_us_32()); 
            prev_signals = curr_signals;
        }
    }
}
