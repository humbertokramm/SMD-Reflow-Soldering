/*
 * Hardware.c
 *
 *  Created on: 28 de abr de 2016
 *      Author: hkramm
 */
#include"msp430g2553.h"

#define RXD				BIT1 //Check your launchpad rev to make sure this is the case. Set jumpers to hardware uart.
#define TXD				BIT2 // TXD with respect to what your sending to the computer. Sent data will appear on this line


//---------------------------------------------------------------
// Initialization code to set up the uart on the experimenter
// board to 8 data,
// 1 stop, no parity, and 9600 baud, polling operation
//---------------------------------------------------------------
void Init_UART(void)
{
	P1SEL = RXD + TXD ;                // Select TX and RX functionality for P1.1 & P1.2
	P1SEL2 = RXD + TXD ;              //

	UCA0CTL1 |= UCSSEL_2;             // Have USCI use System Master Clock: AKA core clk 1MHz

	UCA0BR0 = 104;                    // 1MHz 9600, see user manual
	UCA0BR1 = 0;                      //

	UCA0MCTL = UCBRS0;                // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;             // Start USCI state machine

	//
}
//---------------------------------------------------------------
// Transmite pela serial
//---------------------------------------------------------------
void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;
    while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
    {
        while ((UCA0STAT & UCBUSY)); // Wait if line TX/RX module is busy with data
        UCA0TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
        i++; // Increment variable for array address
    }
}
//---------------------------------------------------------------
// Converte Inteiro em String
//---------------------------------------------------------------
void itoa(long unsigned int value, char* result, int base)
{
  // check that the base if valid
  if (base < 2 || base > 36) { *result = '\0';}

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
	tmp_value = value;
	value /= base;
	*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while ( value );

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while(ptr1 < ptr) {
	tmp_char = *ptr;
	*ptr--= *ptr1;
	*ptr1++ = tmp_char;
  }

}
//---------------------------------------------------------------
// Converte Inteiro em String
//---------------------------------------------------------------
void Init_IOS()
{
	//Entradas
	//P1DIR &= ~BIT3;		// Ensure button is input (sets a 0 in P1DIR register at location BIT3)
	//P1OUT |=  BIT3;		//
	//P1REN |=  BIT3;		// Habilita o resistor de pullup no button

	P1DIR &= ~BIT5;		// Ensure button is input (sets a 0 in P1DIR register at location BIT5)
	P1OUT |=  BIT5;		//
	P1REN |=  BIT5;		// Habilita o resistor de pullup no button

	//Saidas
	P1OUT &= ~(BIT0);
	P1DIR |=  (BIT0);

	P1OUT &= ~(BIT6);
	P1DIR |=  (BIT6);
}
//---------------------------------------------------------------
// Converte Inteiro em String
//---------------------------------------------------------------
short int Read_P1(unsigned int canal)
//short int ReadButtom()
{
	if( (P1IN & canal) == canal ) return 0;
	else return 1;
	return 0;
}
//---------------------------------------------------------------
// ADC10 interrupt service routine
//---------------------------------------------------------------
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
	__bic_SR_register_on_exit(CPUOFF);        // Return to active mode }
}
//---------------------------------------------------------------
// Function containing ADC set-up
//---------------------------------------------------------------
void ConfigureAdc(void)
{
	//BCSCTL1 = CALBC1_1MHZ;			// Set range   DCOCTL = CALDCO_1MHZ;
	//BCSCTL2 &= ~(DIVS_3);			// SMCLK = DCO = 1MHz
	P1SEL |= BIT7;					// ADC input pin P1.7

	ADC10CTL1 = INCH_7 + ADC10DIV_3 ;         // Channel 7, ADC10CLK/3
	//ADC10CTL1 = INCH_4 + ADC10DIV_3 ;         // Channel 4, ADC10CLK/3
	//ADC10CTL1 = INCH_3 + ADC10DIV_3 ;         // Channel 3, ADC10CLK/3

	ADC10CTL0 = SREF_7 + REFON + REFOUT + ADC10SHT_3 + ADC10ON + ADC10IE;  // VREF+ & VREF- as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
	//ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable

	ADC10AE0 |= BIT7;                         // ADC input enable P1.7                      // ADC input enable P1.7

	__enable_interrupt();			// Enable interrupts.
}
//---------------------------------------------------------------
// Lê o ADC
//---------------------------------------------------------------
unsigned int ReadAdc(void)
{
	__delay_cycles(1000);				// Wait for ADC Ref to settle
	ADC10CTL0 |= ENC + ADC10SC;			// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);	// Low Power Mode 0 with interrupts enabled

	return ADC10MEM;				// Assigns the value held in ADC10MEM to the integer called ADC_value
}
//---------------------------------------------------------------
// Converte Inteiro em String
//---------------------------------------------------------------
void OutPut_P1(unsigned int canal, unsigned int cmd)
{
	if(cmd) P1OUT |= canal;
	else P1OUT &= ~canal;
}
//---------------------------------------------------------------
// Converte Inteiro em String
//---------------------------------------------------------------
void Toggle_P1(unsigned int canal)
{
	if( (P1OUT & canal) == canal ) P1OUT &= ~canal;
	else P1OUT |= canal;
}
//---------------------------------------------------------------
// Converte ADC 10Bits em Temperatura
//---------------------------------------------------------------
unsigned int ADC_to_Celcios(unsigned int adc)
{
	float celcios;
	celcios = adc;
	celcios = (celcios-230)*0.320675+22;
	return (unsigned int)celcios;
}
//---------------------------------------------------------------
