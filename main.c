//---------------------------------------------------------------
// Console I/O through the on board UART for MSP 430X4XXX
//---------------------------------------------------------------

#include"msp430g2553.h"
#include"stdio.h"

#define BUTTON			BIT3
#define LED_1			BIT0
#define LED_2			BIT6

#define LED_VERMELHO	BIT0
#define LED_VERDE		BIT6

//Global Variables
unsigned int value;
char charValue[10];
unsigned int listValue[15] = {
		100,	//indice 1
		200,	//indice 2
		300,	//indice 3
		400,	//indice 4
		500,	//indice 5
		600,	//indice 6
		700,	//indice 7
		800,	//indice 8
		900,	//indice 9
		1000,	//indice 10
		800,	//indice 11
		600,	//indice 12
		400,	//indice 13
		200,	//indice 14
		10		//indice 15
};


// Function Prototypes
void Init_UART(void);
void UART_TX(char * tx_data);
void itoa(long unsigned int value, char* result, int base);
void Init_IOS();
short int Read_P1(unsigned int canal);
__interrupt void ADC10_ISR (void);
void ConfigureAdc(void);
unsigned int ReadAdc(void);
void OutPut_P1(unsigned int canal, unsigned int cmd);
void Toggle_P1(unsigned int canal);
//---------------------------------------------------------------
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;           // Desliga o watchdog timer

    Init_IOS(); 						//Configura entradas e saidas

    Init_UART(); 						//Configura a porta serial

    UART_TX("Serial OK\r\n");			//Confirma no terminal o funcionamento da serial

    ConfigureAdc();						//Configura a entrada Analógico/Digital

    Toggle_P1(LED_1);

	for(;;)     // loop infinito
	{
		if(Read_P1(BUTTON)) // A chave foi pressionada?
		{
			unsigned int i = 0, timeDelay;
			for(i = 0 ; i < 14 ; i++)
			{
				timeDelay = 1000;
				while(timeDelay)
				{
					if( (timeDelay == 100)||(timeDelay == 200)||(timeDelay == 400)||(timeDelay == 600)||(timeDelay == 800)||(timeDelay == 1000))
					{
						itoa(listValue[i],charValue,10);	//Converte "value" para a string chamada "charValue"
						UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
						UART_TX("\r\n");  			//Pula a linha no terminal
					}
					value = ReadAdc();
					if(value > listValue[i])
					{
						OutPut_P1(LED_VERMELHO,1);
						OutPut_P1(LED_VERDE,0);
					}
					else if(value < listValue[i])
					{
						OutPut_P1(LED_VERMELHO,0);
						OutPut_P1(LED_VERDE,1);
					}
					else
					{
						OutPut_P1(LED_VERMELHO,0);
						OutPut_P1(LED_VERDE,0);
					}
					if(timeDelay > 0)timeDelay--;
					__delay_cycles(2000); 	//delay simples 1ms

				}
				//itoa(value,charValue,10);	//Converte "value" para a string chamada "charValue"
				//UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
				//UART_TX("\r\n");  			//Pula a linha no terminal
			}
		}
		Toggle_P1(LED_1);
		Toggle_P1(LED_2);
		__delay_cycles(100000); 	//delay simples
	}
}
//---------------------------------------------------------------
