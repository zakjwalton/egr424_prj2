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

uart_dev_t *uart_dev_init(int id, int baud_rate);
int uart_dev_write(uart_dev_t *dev, char *pucBuffer, unsigned long ulCount);
int uart_dev_read(uart_dev_t *dev, char *pucBuffer, unsigned long numChars);

#endif // UART_DEV_H_
