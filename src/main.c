#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "pico/sync.h"

#include "state_logger.h"

#define GPIOS_LEN 11 // the number of gpios.


static spin_lock_t *test;


static inline void log_checker()
{
    uint8_t checker_state = 0;

    // get all bits.
    checker_state |= gpio_get(CHECKER_BIT0) << 0;
    checker_state |= gpio_get(CHECKER_BIT1) << 1;
    checker_state |= gpio_get(CHECKER_BIT2) << 2;


    printf("CHECKER_VALUE: %d\n", checker_state);

    // TODO - check the checker state and log the state.
}

static inline void log_core_standbywfe()
{
    uint8_t core_standbywfe_status = 0;
    // get all bits.
    core_standbywfe_status |= gpio_get(CORE_STANDBYWFE_BIT0) << 0;
    core_standbywfe_status |= gpio_get(CORE_STANDBYWFE_BIT1) << 1;

    // TODO - check the status.
}

static inline void log_core_parity_err() 
{
    uint8_t core_parity_err = 0;
    // get all bits
    core_parity_err |= gpio_get(CORE_PARITY_ERR_BIT0) << 0;
    core_parity_err |= gpio_get(CORE_PARITY_ERR_BIT1) << 1;

    // TODO - check the status
}

static inline void log_watchdog_timer_ex() 
{
    uint8_t watchdog_timer_ex = 0;
    // get all bits.
    watchdog_timer_ex = gpio_get(WATCHDOG_TIMER_EX_BIT);

    // TODO - check the status.
}

static inline void log_forced_standby() 
{
    uint8_t forced_standby_status = 0;
    forced_standby_status |= gpio_get(FORCED_STANDBY_BIT0) << 0;
    forced_standby_status |= gpio_get(FORCED_STANDBY_BIT1) << 1;
    // TODO - check the status.
}

static inline void log_checkpoint_err_after_bt()
{
    uint8_t checkpoint_err_after_bt = 0;
    checkpoint_err_after_bt = gpio_get(CHECKPOINT_ERR_AFTER_BT_BIT);

    // TODO - check the bit.
}

// Interrupt service routine for the gpios.
static void state_logger_gpio_isr(uint gpio, uint32_t event_mask) {
    if (IS_CHECKER(gpio)) {
        log_checker();
    } else if (IS_CORE_STANDBYWFE(gpio)) {
        log_core_standbywfe();
    } else if (IS_CORE_PARITY_ERR(gpio)) {
        log_core_parity_err();
    } else if (IS_WATCHDOG_TIMER_EX(gpio)) {
        log_watchdog_timer_ex();
    } else if (IS_FORCED_STANDBY(gpio)) {
        log_forced_standby();
    } else if (IS_CHECKPOINT_ERR_AFTER_BT(gpio)) {
        log_checkpoint_err_after_bt();
    }
}

// Initialize all the gpios to be used.
static void state_logger_init_gpios()
{
    sleep_ms(5000);
    for (int gpio = 0; gpio < GPIOS_LEN; gpio++) {
        // initilize current gpio
        gpio_init(gpio);
        // set current gpio to input mode.
        gpio_set_dir(gpio, GPIO_IN);
        // enable interrups for each bpio.
        if (0 == gpio) {
            gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | 
                                                     GPIO_IRQ_EDGE_FALL, 
                                                     true, 
                                                     &state_logger_gpio_isr);
        } else {
            gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
        }
    }
}

int main(void)
{
    sleep_ms(10000);
    test = spin_lock_init(50);

    stdio_init_all();
    

    state_logger_init_gpios();

    while (true);
}
