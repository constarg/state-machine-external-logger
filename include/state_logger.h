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

// Bits that are reserved for checker.
#define CHECKER_BIT0                 0x0 // GPIO - 0
#define CHECKER_BIT1                 0x1 // GPIO - 1
#define CHECKER_BIT2                 0x2 // GPIO - 2

// Bits that are reserved for core_standbywfe signal.
#define CORE_STANDBYWFE_BIT0         0x3 // GPIO - 3
#define CORE_STANDBYWFE_BIT1         0x4 // GPIO - 4

// Bits that are reserved for core_parity_error signal.
#define CORE_PARITY_ERR_BIT0         0x5 // GPIO - 5
#define CORE_PARITY_ERR_BIT1         0x6 // GPIO - 6

// Bits that are reserved for watchdog_timer_expiration signal.
#define WATCHDOG_TIMER_EX_BIT        0x7 // GPIO - 7

// Bits that are reserved for forced_standby_status signal.
#define FORCED_STANDBY_BIT0          0x8 // GPIO - 8
#define FORCED_STANDBY_BIT1          0x9 // GPIO - 9

// Bits that are reserved for checkpoint_error_after_boot signal.
#define CHECKPOINT_ERR_AFTER_BT_BIT  0xA // GPIO - 10

// Check who is the owner of the signal.
// is checker?
#define IS_CHECKER(gpio) \
    (CHECKER_BIT0 == (gpio) || CHECKER_BIT1 == (gpio) || CHECKER_BIT2 == (gpio))

// is from core_standbywfe?
#define IS_CORE_STANDBYWFE(gpio) \
    (CORE_STANDBYWFE_BIT0 == (gpio) || CORE_STANDBYWFE_BIT1 == (gpio))

// is from core_parity_error?
#define IS_CORE_PARITY_ERR(gpio) \
    (CORE_PARITY_ERR_BIT0 == (gpio) || CORE_PARITY_ERR_BIT1 == (gpio))

// is from watchdog_timer_expiration?
#define IS_WATCHDOG_TIMER_EX(gpio) \
    (WATCHDOG_TIMER_EX_BIT == (gpio))

// is from forced_standby_status?
#define IS_FORCED_STANDBY(gpio) \
    (FORCED_STANDBY_BIT0 == (gpio) || FORCED_STANDBY_BIT1 == (gpio)) 

// is from checkpoint_error_after_boot?
#define IS_CHECKPOINT_ERR_AFTER_Bt(gpio) \
    (CHECKPOINT_ERR_AFTER_BT_BIT == (gpio))

#endif
