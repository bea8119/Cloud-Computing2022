#include "main.h"
#include "kprintf.c"

/**
 * This is the C entry point, upcalled once the hardware has been setup properly
 * in assembly language, see the reset.s file.
 */

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char*) s1 - *(const unsigned char*) s2;
}

void _start() {

	int j;
	int orizontal;
	int vertical;
	int r;
	int rows_number = 0;
	int history_number = 0;
	char line[100];
	char command[5];
	char rimanente[100];
	char history[20][100];
	int irq = 12;

	vic_setup();
	vic_enable();
	void *cookie = NULL;
	uart_init(UART0);

	vic_irq_enable(irq, (*uart_rx_handler), cookie);

	/* while (1) {
	 unsigned char c;
	 while (0 == uart_receive(UART0, &c)) {

	 // friendly reminder that you are polling and therefore spinning...
	 // not good for the planet! But until we introduce interrupts,
	 // there is nothing you can do about it... except comment out
	 // this annoying code ๐

	 count++;
	 if (count > 10000000) {
	 //uart_send_string(UART0, "Zzz....");
	 count = 0;
	 }
	 }

	 if (c == '\r')
	 uart_send(UART0, '\n');
	 kprintf("Il codice ASCII รจ: %d", c);


	 }*/

	j=0;
	orizontal=0;
	vertical=0;

	while (1) {

		uint8_t c;
		while (1 == uart_receive(UART0, &c)) {
			if (c == 13) {

				r = 0;
				line[j] = '\0';
				while (line[r] != '\0') {
					while (r < 4) {
						command[r] = line[r];
						r++;
					}
					r++;
					rimanente[r - 5] = line[r];

				}
				rimanente[r] = '\0';

				if (strcmp("reset", line) == 0) {
					for (r = 0; r <= rows_number; r++) {
						kprintf("\033[2K\r");
						kprintf("\33[1A");
					}

					rows_number = -1;
				}

				else if (strcmp("echo", command) == 0) {
					uart_send_string(UART0, "\n\r");
					uart_send_string(UART0, rimanente);
					rows_number++;

				}
				uart_send_string(UART0, "\n\r");
				for (r = 0; r <= j; r++)
					history[history_number % 20][r] = line[r];

				j = 0;
				vertical = 0;
				orizontal = 0;
				rows_number++;
				history_number++;
			} else if (c == 127) {
				kprintf("\033[2K\r");

				if (orizontal == j) {
					line[j--] = ' ';
					orizontal = j;
				}

				else {
					for (r = orizontal - 1; r < j; r++)
						line[r] = line[r + 1];
					line[j--] = ' ';
					orizontal--;
				}
				for (r = 0; r < j; r++)
					kprintf("%c", line[r]);
				for (r = 0; r < j - orizontal; r++) {
					kprintf("\033[1D");
				}

			} else if (c == 27) {
				uart_receive(UART0, &c);
				uart_receive(UART0, &c);

				if (c == 'A') { //up arrow
					if (vertical < 20 && vertical < history_number)
						vertical++;
					kprintf("\033[2K\r");
					kprintf("%s", history[(history_number - vertical) % 20]);
					r = 0;
					while (history[(history_number - vertical) % 20][r] != '\0') {
						line[r] = history[(history_number - vertical) % 20][r];
						r++;
					}
					j = r;
				}

				else if (c == 'B') { //down arrow
					if (vertical > 0)
						vertical--;
					kprintf("\033[2K\r");
					kprintf("%s", history[(history_number - vertical) % 20]);
					r = 0;
					while (history[(history_number - vertical) % 20][r] != '\0') {
						line[r] = history[(history_number - vertical) % 20][r];
						r++;
					}
					j = r;
				}

				else if (c == 'C') { //right arrow
					if (orizontal < j)
						orizontal++;
					kprintf("\033[1C");
				}

				else if (c == 'D') { //left arrow
					if (orizontal > 0) {
						orizontal--;
						kprintf("\033[1D");
					}
				}

			} else {
				if (orizontal == j) {
					line[orizontal] = c;
					uart_send(UART0, c);
					j++;
					orizontal = j;
				} else {
					kprintf("\033[2K\r");
					for (r = j; r > orizontal; r--)
						line[r] = line[r - 1];
					line[orizontal] = c;
					orizontal++;
					j++;
					for (r = 0; r < j; r++)
						kprintf("%c", line[r]);
					for (r = 0; r < j - orizontal; r++)
						kprintf("\033[1D");

				}
			}


		}
		wfi();
	}
}

