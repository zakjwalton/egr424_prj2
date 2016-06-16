#ifndef UART_DEV_H_
#define UART_DEV_H_

#include "utils/ringbuf.h"

#define UART_BUFFER_SIZE 128

typedef volatile struct uart_dev {
    unsigned long base_addr;
    unsigned char tx[UART_BUFFER_SIZE];
    unsigned char rx[UART_BUFFER_SIZE];
    tRingBufObject tx_buf;
    tRingBufObject rx_buf;
} uart_dev_t;

void uart_dev_init(uart_dev_t *dev, unsigned long base_addr);
void uart_dev_send(uart_dev_t *dev, const char *pucBuffer, unsigned long ulCount);

#endif // UART_DEV_H_
