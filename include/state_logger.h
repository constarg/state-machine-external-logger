#ifndef STATE_LOGGER_H
#define STATE_LOGGER_H

/**
 * GPIO 0 through 2 -> checker.
 * GPIO 3 through 4 -> core_standbywfe
 * GPIO 5 through 6 -> core_parity_error
 * GPIO 7           -> watchdog_timer_expiration
 * GPIO 8 through 9 -> forced_standby_status
 * GPIO 10          -> checkpoint_error_after_boot
 */

// Check who is the owner of the signal.
// is checker?
#define IS_CHECKER(gpio) \
    (0 == (gpio) || 1 == (gpio) || 2 == (gpio))

// is from core_standbywfe?
#define IS_CORE_STANDBYWFE(gpio) \
    (3 == (gpio) || 4 == (gpio))

// is from core_parity_error?
#define IS_CORE_PARITY_ERR(gpio) \
    (5 == (gpio) || 6 == (gpio))

// is from watchdog_timer_expiration?
#define IS_WATCHDOG_TIMER_EX(gpio) \
    (7 == (gpio))

// is from forced_standby_status?
#define IS_FORCED_STANDBY(gpio) \
    (8 == (gpio) || 9 == (gpio)) 

// is from checkpoint_error_after_boot?
#define IS_CHECKPOINT_ERR_AFTER_BT(gpio) \
    (10 == (gpio))

#endif
