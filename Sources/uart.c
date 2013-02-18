#include <Core/Core.h>
#include <Platform/Platform.h>

#include "uart.h"

static unsigned char uart_ready = 0;

void uart_init(int port) {
  outb(0x00, port + 1);    // Disable all interrupts
  outb(0x80, port + 3);    // Enable DLAB (set baud rate divisor)

  //outb(0x0C, port + 0);  // Set divisor to 12 (lo byte) 9600 baud
  //outb(0x00, port + 1);  //                 0 (hi byte)
  outb(0x01, port + 0);  // Set divisor to 1 (lo byte) 115200 baud
  outb(0x00, port + 1);  //                0 (hi byte)

  outb(0x03, port + 3);    // 8 bits, no parity, one stop bit
  outb(/*0xC7*/ 0x00, port + 2);    // Enable FIFO, clear them, with 14-byte threshold
  outb(/*0x0B*/ 0x08, port + 4);    // IRQs enabled, RTS/DSR set

  outb(0x0d, port + 1); // enable all intrs but writes
}

void uart_write(int port, char c) {
  if(!uart_ready) {
    uart_init(port);
    uart_ready = 1;
  }
  while ((inb(port + 5) & 0x20) == 0);
  outb(c, port);
}

void uart_read(int port) {
  while ((inb(port + 5) & 1) == 0);
  return inb(port);
}

