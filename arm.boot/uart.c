#include "main.h"

/**
 * Receive a character from the given uart, this is a non-blocking call.
 * Returns 0 if there are no character available.
 * Returns 1 if a character was read.
 */

struct cb* cb;

int uart_receive(int uart, unsigned char *s) {
  unsigned short* uart_fr = (unsigned short*) (uart + UART_FR);
  unsigned short* uart_dr = (unsigned short*) (uart + UART_DR);


  if(!cb_empty(cb)){
	  cb_get(cb, s);
	  return 1;

  }else return 0;

  /*
  if (*uart_fr & UART_RXFE)
    return 0;
  *s = (*uart_dr & 0xff);
  return 1; */
}


/**
 * Sends a character through the given uart, this is a blocking call.
 * The code spins until there is room in the UART TX FIFO queue to send
 * the character.
 */
void uart_send(int uart, unsigned char s) {
  unsigned short* uart_fr = (unsigned short*) (uart + UART_FR);
  unsigned short* uart_dr = (unsigned short*) (uart + UART_DR);
  while (*uart_fr & UART_TXFF)
    ;
  *uart_dr = s;
}

/**
 * This is a wrapper function, provided for simplicity,
 * it sends a C string through the given uart.
 */
void uart_send_string(int uart, const unsigned char *s) {
  while (*s != '\0') {
    uart_send(uart, s);
    s++;
  }
}

void uart_init(int uart){
	// Enable FIFO queues, both rx-queue and tx-queue.
	  uint16_t lcr = *(uint16_t*) (uart + CUARTLCR_H);
	  lcr |= CUARTLCR_H_FEN;
	  *(uint16_t*) (uart + CUARTLCR_H) = lcr;
	  uint16_t reg = *(uint16_t*)(uart+UART_IMSC);
	  reg = reg | UART_IMSC_RXIM;
	  *(uint16_t*)(uart+UART_IMSC) = reg;

	  cb_init(cb);
}

void uart_rx_handler(void* cookie){
	uint8_t c;
	 unsigned short* uart_dr = (unsigned short*) (UART0 + UART_DR);
	 if(!cb_full(cb)){
	 c = (*uart_dr & 0xff);
	 cb_put(cb, c);
	 }

}


