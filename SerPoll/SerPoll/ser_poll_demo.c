#include "qsk_bsp.h"

/* 	This program demonstrates serial I/O using polling. 
	It communicates via UART0 at 19200 baud, 8 data bits
	no parity bit and one stop bit. UART0's TxD is at P6_3
	and RxD is at P6_2.

	demo1() sends out characters as fast as possible.
	
	demo2() sends out strings.
	
	demo3() echoes all characters received on the UART,
	adding one to the character if switch S2 is pressed.
*/

/* Prototype declarations */
void init_UART0(void);
void demo1(void);
void demo2_ser_tx(_far char * buf);
void demo2(void);
void demo3(void);

void delay(long unsigned int n){
  while (n--);
}

void init_UART0() {
  // UART 0 baud rate generator
  u0brg = (unsigned char) (f1_CLK_SPEED/(16*19200)-1);

  // UART 0 transmit/receive mode register
  smd2_u0mr = 1;   // eight data bits
  smd1_u0mr = 0; 
  smd0_u0mr = 1;
  ckdir_u0mr = 0; // internal clock
  stps_u0mr = 0;
  pry_u0mr = 0; 
  prye_u0mr = 0; // no parity

  // uart0 t/r control register 0
  clk1_u0c0 = 0; // select f/1 clock
  clk0_u0c0 = 0;
  nch_u0c0 = 0; // CMOS push-pull output
  ckpol_u0c0 = 0; // required
  uform_u0c0 = 0; // required
  crs_u0c0 = 0; // required
  crd_u0c0 = 1; // required

  // uart0 t/r control register 1
  te_u0c1 = 1; // enable transmitter
  re_u0c1 = 1; // enable receiver

  // uart t/r control register 2
  u0irs = 0;   // select interrupt source
  u1rrm = 0;   // select interrupt source
  clkmd0 = 0;  // n/a
  clkmd1 = 0;  // n/a
  rcsp=1;   // rxdo port to p6_2
   
}

void demo2_ser_tx(_far char * buf) {
  // polled buffer transmit demo
  // transmit a null-terminated string
  while (*buf) {
    while (!ti_u0c1); // wait for transmitter empty
    u0tbl = *buf; // load character into buffer
    buf++; // increment buffer pointer
  }
}

void demo1(void) {
  // polled transmit demo
  unsigned long ctr;
  char c='A';
  while (1) {
    while (!ti_u0c1); // wait for transmitter empty
    for (ctr=0; ctr<1000; ctr++);
    u0tbl = c;
    c++;
    if (c>'z')
      c = 'A';
  }
}

void demo2(void) {
  while (1) {
    demo2_ser_tx("Testing!\n\r");
    delay(100000);
  }
}

void demo3(void) {
  //  polling echo example
  char c;
  while (1) {
    while (!ri_u0c1)
      ;
    c = u0rbl;
    if (!S2) { // "Encrypt" response
      c += 1;
      YLW_LED = LED_ON;
    } else
      YLW_LED = LED_OFF;
    
    while (!ti_u0c1)
      ;
    u0tbl = c;

    /* cr/lf translation for terminal */
    if (c == '\r') {
      while (!ti_u0c1)
;
      u0tbl = '\n';
    }
  }
}

void main(void) {
  mcu_init();

  /* LED initialization for this demo - macro defined in skp_bsp.h */
  ENABLE_LEDS;
  /* Switch initialization for this demo, define in skp_bsp.h */
  ENABLE_SWITCHES;

  init_UART0(); // 19200 baud. Transmitted data is on P6_3, Received data is on P6_2.
  RED_LED=LED_ON;
  //demo1();
  demo2();
  //demo3();
}
