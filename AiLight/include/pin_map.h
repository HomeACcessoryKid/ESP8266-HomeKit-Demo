#ifndef __PIN_MAP_H__
#define __PIN_MAP_H__

#include "c_types.h"
#include "gpio.h"

#define GPIO_PIN_NUM 13
#define GPIO_MAX_INDEX 16

extern uint8_t pin_func[GPIO_MAX_INDEX + 1];
extern uint32_t pin_name[GPIO_MAX_INDEX + 1];
#ifdef GPIO_INTERRUPT_ENABLE
extern GPIO_INT_TYPE pin_int_type[GPIO_MAX_INDEX + 1];
#endif

static inline int is_gpio_invalid(unsigned gpio)
{
    if (gpio > GPIO_MAX_INDEX) {
        return 1;
    }
    switch (gpio) {
    case 6:
    case 7:
    case 8:
    case 11:
        return 1;
    default:
        return 0;
    }

}
#endif // #ifndef __PIN_MAP_H__
