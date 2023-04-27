#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
// GPIO handler assembly.
#include "gpio_handler.pio.h"

/**
 * This function sends to the usb serial cable 2 bytes that 
 * represents the values of each signal of the state machine.
 *
 * @param src The signals to send.
 */
static inline void send_signals_to_usb(const uint32_t src)
{
    // Build the two bytes based on the specification.
    uint8_t first_byte  = src & 0xFF;
    uint8_t second_byte = src >> 8;

    putchar(first_byte);
    putchar(second_byte);
}

int main(void)
{
    // Important variables.
    PIO pio                  = pio0;            // The PIO instance/block to use.
    uint offset              = 0;               // The offset to the instruction memory of the state machine.
    uint sm                  = 0;               // The state machine to be used from the pio instance.
    // The variables down below are used to get the next correct value of GPIOS.
    uint32_t curr_signals    = 0;               // The current values of the GPIOS, the two samples are in the same variable.
    uint32_t curr_signals_s1 = 0;               // The current values of the GPIOS, sample 1.
    uint32_t curr_signals_s2 = 0;               // The current values of the GPIOS, sample 2.
    uint32_t prev_signals    = 0;               // The previous values of the GPIOS.
    // Has any new state occured?
    bool checker_chg         = 0;               // Determines if the checker has changed.
    bool rest_signals_chg    = 0;               // Determines if any other signal after the first 5 bits has changed.
    uint8_t curr_checker_val = 0;               // current checker value.
    uint8_t prev_checker_val = 0;               // previous checker value.

    stdio_init_all();

    // increase the frequency of the system clock to its maximum potentials.
    set_sys_clock_khz(133000, true);
   
    // initialize and start the pio (programmaple io).
    offset = pio_add_program(pio, &gpio_handler_program);
    gpio_handler_program_init(pio, sm, offset, 0);
    pio_sm_set_enabled(pio, sm, true);
    
    while (true) {
        curr_signals    = pio_sm_get_blocking(pio, sm); // block until a new value.
        curr_signals_s1 = curr_signals & 0x0000FFFF;
        curr_signals_s2 = curr_signals >> 16;
        // Get the correct value.
	// and bitwise oparation must be performed only on checker.
        curr_signals = curr_signals_s1 & (0xFFF8 | (curr_signals_s2 & 0x7));
        if ((curr_signals & 0x7) == 0) {
            curr_signals = ((curr_signals_s1 & 0x7) > 0)? curr_signals_s1:curr_signals_s2;
        }
        
        curr_checker_val = curr_signals & 0x7; 
        prev_checker_val = prev_signals & 0x7;
        
        // The statement down below is used to determine if the checker has changed.
        checker_chg = curr_checker_val != prev_checker_val;
        // The statement down below is used to determine if any other signal has changed. 
        rest_signals_chg = (curr_signals & 0xFFE0) != (prev_signals & 0xFFE0);
        // Check if the checker state changed or if the rest of the signals are changed.
        if (checker_chg || rest_signals_chg) {
            send_signals_to_usb(curr_signals);
            prev_signals = curr_signals;
        } 
    }
}
