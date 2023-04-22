#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
// GPIO handler assembly.
#include "gpio_handler.pio.h"


#define GPIOS_LEN 16 // The number of used GPIOS.

/**
 * This function sends to the usb serial cable 2 bytes that 
 * represent the values of each signal of the state machine.
 *
 * @param src The signals to send.
 */
static inline void send_signals_to_usb(const uint32_t src)
{
    // Build the two bytes based on the specification.
    uint8_t first_byte = src & 0xFF;
    uint8_t second_byte = src >> 8;

    putchar(first_byte);
    putchar(second_byte);
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
    
    bool was_rollback = 0; // Determine if the previous signal was rollback.
    uint32_t tmp_signals = 0; // In this variable is stored a signal that is rollback. We don't transmit the signal until we verify that rollback is correct.

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
            if ((curr_signals & 0x7) == 0x3) {
                was_rollback = true;
                tmp_signals = curr_signals;
            } else if (was_rollback) {
                if ((curr_signals & 0x7) == 0x4) {
                    send_signals_to_usb(tmp_signals);
                    send_signals_to_usb(curr_signals);
                }
                was_rollback = false;
                send_signals_to_usb(curr_signals);
            } else {
                send_signals_to_usb(curr_signals);
            }
            prev_signals = curr_signals;
        }
    }
}
