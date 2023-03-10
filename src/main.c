#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "state_logger.h"

#define GPIOS_LEN 11 // the number of gpios.




void state_logger_gpio_isr(uint gpio, uint32_t event_mask) {
    printf("%d\n", gpio);
    if (IS_CHECKER(gpio)) {
        printf("CHECKER\n");
    } else if (IS_CORE_STANDBYWFE(gpio)) {
        printf("CORE SDB\n");
    } else if (IS_CORE_PARITY_ERR(gpio)) {
        printf("CORE PARITY\n");
    } else if (IS_WATCHDOG_TIMER_EX(gpio)) {
        printf("WATCHDOG\n");
    } else if (IS_FORCED_STANDBY(gpio)) {
        printf("FORCED\n");
    } else if (IS_CHECKPOINT_ERR_AFTER_BT(gpio)) {
        printf("CHECKPOINT\n");
    }
}

// Initialize all the gpios to be used.
static void state_logger_init_gpios()
{
    for (int gpio = 0; gpio < GPIOS_LEN; gpio++) {
        printf("OK -> %d\n", gpio);
        // initilize current gpio
        gpio_init(gpio);
        // set current gpio to input mode.
        gpio_set_dir(gpio, GPIO_IN);
        // enable interrups for each bpio.
        gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | 
                                                 GPIO_IRQ_EDGE_FALL, 
                                           true, 
                                           &state_logger_gpio_isr);
    }
}

int main(void)
{
    stdio_init_all();

    state_logger_init_gpios();
    //
         //   gpio_init(2);
        // set current gpio to input mode.
        //gpio_set_dir(2, GPIO_IN);
        // enable interrups for each bpio.
        /*gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_RISE, 
                                           true, 
                                           &gpio_isr);*/


    while (true) {
        //printf("ALL OK\n");
        //sleep_ms(2000);
    }
}
