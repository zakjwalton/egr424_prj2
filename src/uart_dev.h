#ifndef UART_DEV_H_
#define UART_DEV_H_

#include "ring.h"

#define UART_BUFFER_SIZE 128

typedef struct uart_dev {
    unsigned long base_addr;
    rb_t *tx;
    rb_t *rx;
} uart_dev_t;

uart_dev_t *uart_dev_create(unsigned long base_addr);

#endif // UART_DEV_H_
