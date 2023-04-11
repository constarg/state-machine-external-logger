#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
// GPIO handler assembly.
#include "gpio_handler.pio.h"


#define GPIOS_LEN 16 // The number of used GPIOS.

static inline void print_signals_2(uint32_t src, int len)
{
    for (int sig = 0; sig < len; sig++) {
        printf("%d", (src >> sig) & (0x1));
    }
    printf("\n");
}

static inline void print_signals(uint32_t src)
{
    // reposition the signal order to much the specification.
    uint8_t first_byte = src & 0xFF;
    uint8_t second_byte = src >> 8;

    printf("%c", first_byte);
    printf("%c", second_byte);
}

int main(void)
{
    PIO pio = pio0; // The PIO instance/block to use.
    uint offset = 0; // The offset to the instruction memory of the state machine.
    uint sm = 0; // The state machine to be used from the pio instance.
    uint32_t curr_signals = 0; // The current values of the GPIOS.
    uint32_t prev_signals = 0; // The previous values of the GPIOS.
    uint8_t shift_amount = 32 - GPIOS_LEN; // How many bits to shift right.

    bool checker_chg = 0; // Determines if the checker has changed.
    bool rest_signals_chg = 0; // Determines if any other signal after the first 5 bits has changed.

    stdio_init_all();

    // increase the system clock to it's maximum potentials.
    set_sys_clock_khz(133000, true);
    
    offset = pio_add_program(pio, &gpio_handler_program);
    gpio_handler_program_init(pio, sm, offset, 0);
    pio_sm_set_enabled(pio, sm, true);

    while (true) {
        curr_signals = (pio_sm_get_blocking(pio, sm) >> shift_amount); // block until a new value.
        // The below statement used to determine if the checker has changed.
        checker_chg = (curr_signals & 0x7) != (prev_signals & 0x7);
        // The below statement used to determine if any other signal after standbywfe has changed. 
        rest_signals_chg = (curr_signals & 0xFFE0) != (prev_signals & 0xFFE0);

        if (checker_chg || rest_signals_chg) {
            print_signals(curr_signals); 
            prev_signals = curr_signals;
        }
    }
}
