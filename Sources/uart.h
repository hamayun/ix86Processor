#ifndef _UART_H_
#define _UART_H_

#define UART_COM1 0x3f8

#define DEBUGGER_COM_PORT UART_COM1

void uart_init(int port);

void uart_write(int port, char c);

#endif

