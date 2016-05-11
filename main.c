//---------------------------------------------------------------
// Console I/O through the on board UART for MSP 430X4XXX
//---------------------------------------------------------------

#include"msp430g2553.h"
#include"stdio.h"

#define BUTTON			BIT5
#define LED_1			BIT0
#define LED_2			BIT6

#define LED_VERMELHO	BIT0
#define LED_VERDE		BIT6

//Global Variables
unsigned int value;
char charValue[10];

unsigned int listValue[3][2] = {
		{150,180},	//indice 1
		{175,0},	//indice 2
		{240,10}	//indice 3
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
unsigned int ADC_to_Celcios(unsigned int adc);
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
    	value = ReadAdc();

    	itoa(value,charValue,10);	//Converte "value" para a string chamada "charValue"
		UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
		UART_TX("\t");  			//Pula a linha no terminal

		value = ADC_to_Celcios(value);
		itoa(value,charValue,10);	//Converte "value" para a string chamada "charValue"
		UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
		UART_TX(" °C\r\n");  			//Pula a linha no terminal

		if(Read_P1(BUTTON)) // A chave foi pressionada?
		{
			Toggle_P1(LED_1);
			Toggle_P1(LED_2);
			__delay_cycles(100000); 	//delay simples
		}


    	__delay_cycles(200000); 	//delay simples 200ms

	}


/*
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
	}*/
}
//---------------------------------------------------------------
