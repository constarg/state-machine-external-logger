#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
// GPIO handler assembly.
#include "gpio_handler.pio.h"
#include "checker_validator.pio.h"


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

    uint8_t checker_xord = 0; // The value of the checker after the xor oparation.
    uint8_t curr_checker_val = 0; // current checker value.
    uint8_t prev_checker_val = 0; // previous checker value.

    stdio_init_all();

    // increase the system clock to it's maximum potentials.
    set_sys_clock_khz(133000, true);
   
    // initialize and start the pio (programmaple io).
    offset = pio_add_program(pio, &gpio_handler_program);
    gpio_handler_program_init(pio, sm, offset, 0);
    pio_sm_set_enabled(pio, sm, true);

    while (true) {
        curr_signals = (pio_sm_get_blocking(pio, sm) >> shift_amount); // block until a new value.
        curr_checker_val = curr_signals & 0x7; 
        prev_checker_val = prev_signals & 0x7;
        // The below statement used to determine if the checker has changed.
        checker_chg = curr_checker_val != prev_checker_val;
        // The below statement used to determine if any other signal after standbywfe has changed. 
        rest_signals_chg = (curr_signals & 0xFFE0) != (prev_signals & 0xFFE0);

        // Check if the checker state changed or if the rest of the signals changed.
        if (checker_chg) {
            checker_xord = curr_checker_val ^ prev_checker_val;
            // Do not allow those special cases. 
            if ((checker_xord == 4 && curr_checker_val != 0) ||
                (checker_xord == 3 && prev_signals == 0) || 
                (checker_xord == 6 && prev_signals == 0)) {
                continue;
            // In general allow a signal
            } else if (checker_xord == 1 || checker_xord > 2) {
                send_signals_to_usb(curr_signals);
                prev_signals = curr_signals;
            }
        } else if (rest_signals_chg) {
            send_signals_to_usb(curr_signals);
            prev_signals = curr_signals;
        }
    }
}
