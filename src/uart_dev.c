#include "inc/hw_types.h"
#include "drivers/rit128x96x4.h"
#include "uart_dev.h"

uart_dev_t *uart_dev_create(unsigned long base_addr) {
    uart_dev_t *uart_dev;
    uart_dev = malloc(sizeof(uart_dev_t));
    uart_dev->tx = rb_create(UART_BUFFER_SIZE);
    uart_dev->rx = rb_create(UART_BUFFER_SIZE);
    uart_dev->base_addr = base_addr;

    return uart_dev;
}
