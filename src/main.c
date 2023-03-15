#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "pico/sync.h"

#include "state_logger.h"

#define GPIOS_LEN 11 // the number of gpios.

// print the encoded information.
#define LOG_SIGNAL_ENC(signal, state) \
    printf("[LOG] [%s] : [OUTPUT] --> %s\n", signal, state);

// print the value of a signal that can't be encoded.
#define LOG_SIGNAL_VALUE(signal, value) \
    printf("[LOG] [%s] : [OUTPUT] --> %X\n", signal, value);

// The privious state of the checker.
static uint8_t g_checker_prev_state = 0;

// Checker states.
static char *g_checker_states[8] = {
    "IDLE", "CHECK STATUS", "CHECKPOINT_FROM_BOTH_CORES",
    "ROLLBACK", "WAKE_UP_CORES", "WAIT_FOR_STANDBY",
    "CHECKPOINT_FROM_ONE_CORE", "ROLLFORWARD"
};

// The previous value of core core standbywfe
static uint8_t g_prev_core_standbywfe = 0;

// The previous value of core parity error.
static uint8_t g_prev_core_parity_error = 0;

// The previous value of watchdog timer expiration
static uint8_t g_prev_watchdog_timer_ex = 0;

// The previous value of forced standby
static uint8_t g_prev_forced_standby = 0;

// The previous value of checkpoint after boot.
static uint8_t g_prev_checkpoint_err_after_bt = 0;


static inline void log_checker()
{
    uint8_t checker_state = 0;
    uint32_t intr_state; // The state of the interrupts.

    intr_state = save_and_disable_interrupts(); // save interrupt state.
    // get all bits.
    checker_state |= gpio_get(CHECKER_BIT0) << 0;
    checker_state |= gpio_get(CHECKER_BIT1) << 1;
    checker_state |= gpio_get(CHECKER_BIT2) << 2;

    if (g_checker_prev_state == checker_state) {
        restore_interrupts(intr_state);
        return;
    } else {
        g_checker_prev_state = checker_state;
        //LOG_SIGNAL_ENC("CHECKER", g_checker_states[checker_state]);
        LOG_SIGNAL_VALUE("CHECKER", checker_state); // TODO - remove this.
        restore_interrupts(intr_state);
    }
}

static inline void log_core_standbywfe()
{
    uint8_t core_standbywfe_status = 0;
    uint32_t intr_state = 0;
    
    // disable interrupts.
    intr_state = save_and_disable_interrupts(); // save interrupt state.
    // get all bits.
    core_standbywfe_status |= gpio_get(CORE_STANDBYWFE_BIT0) << 0;
    core_standbywfe_status |= gpio_get(CORE_STANDBYWFE_BIT1) << 1;

    if (g_prev_core_standbywfe == core_standbywfe_status) {
        restore_interrupts(intr_state);
        return;
    } else {
        g_prev_core_standbywfe = core_standbywfe_status;
        LOG_SIGNAL_VALUE("CORE_STANDBYWFE", core_standbywfe_status);
        restore_interrupts(intr_state);
    }
}

static inline void log_core_parity_err() 
{
    uint8_t core_parity_err = 0;
    uint32_t intr_state = 0;
    
    intr_state = save_and_disable_interrupts();
    // get all bits
    core_parity_err |= gpio_get(CORE_PARITY_ERR_BIT0) << 0;
    core_parity_err |= gpio_get(CORE_PARITY_ERR_BIT1) << 1;
    
    if (g_prev_core_parity_error == core_parity_err) {
        restore_interrupts(intr_state);
        return;
    } else {
        g_prev_core_parity_error = core_parity_err;
        LOG_SIGNAL_VALUE("CORE_PARITY_ERR", core_parity_err);
        restore_interrupts(intr_state);
    }
}

static inline void log_watchdog_timer_ex() 
{
    uint8_t watchdog_timer_ex = 0;
    uint32_t intr_state = 0;
    intr_state = save_and_disable_interrupts();
    // get all bits.
    watchdog_timer_ex = gpio_get(WATCHDOG_TIMER_EX_BIT);
    if (watchdog_timer_ex != g_prev_watchdog_timer_ex) {
        g_prev_watchdog_timer_ex = watchdog_timer_ex;
        LOG_SIGNAL_VALUE("WATCHDOG_TIMER_EXPIRED", watchdog_timer_ex);
    }
    restore_interrupts(intr_state);
}

static inline void log_forced_standby() 
{
    uint8_t forced_standby_status = 0;
    uint32_t intr_state = 0;
    intr_state = save_and_disable_interrupts();
    forced_standby_status |= gpio_get(FORCED_STANDBY_BIT0) << 0;
    forced_standby_status |= gpio_get(FORCED_STANDBY_BIT1) << 1;

    if (g_prev_forced_standby == forced_standby_status) {
        restore_interrupts(intr_state);
        return;
    } else {
        g_prev_forced_standby = forced_standby_status;
        LOG_SIGNAL_VALUE("FORCED_STANDBY", forced_standby_status);
        restore_interrupts(intr_state);
    }
}

static inline void log_checkpoint_err_after_bt()
{
    uint8_t checkpoint_err_after_bt = 0;
    uint32_t intr_state = 0;
    checkpoint_err_after_bt = gpio_get(CHECKPOINT_ERR_AFTER_BT_BIT);
    intr_state = save_and_disable_interrupts();
    if (checkpoint_err_after_bt != g_prev_checkpoint_err_after_bt) {
        g_prev_checkpoint_err_after_bt = checkpoint_err_after_bt;
        LOG_SIGNAL_VALUE("CHECKPOINT_ERROR_AFTER_BOOT", checkpoint_err_after_bt);
    }
    restore_interrupts(intr_state);
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
