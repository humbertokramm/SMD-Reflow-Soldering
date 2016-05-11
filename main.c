//---------------------------------------------------------------
// Console I/O through the on board UART for MSP 430X4XXX
//---------------------------------------------------------------

#include"msp430g2553.h"
#include"stdio.h"

#define BUTTON_S1		BIT3
#define BUTTON_S3		BIT5

#define LED_1			BIT0
#define LED_2			BIT6

#define LED_VERMELHO	BIT6
#define LED_VERDE		BIT0

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
unsigned int ADC_to_Celsius(unsigned int adc);
//---------------------------------------------------------------
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;           // Desliga o watchdog timer

    Init_IOS(); 						//Configura entradas e saidas

    Init_UART(); 						//Configura a porta serial

    UART_TX("Serial OK\r\n");			//Confirma no terminal o funcionamento da serial

    ConfigureAdc();						//Configura a entrada Analógico/Digital


    for(;;)     // loop infinito
	{
    	value = ReadAdc();

    	itoa(value,charValue,10);	//Converte "value" para a string chamada "charValue"
		UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
		UART_TX("\t");  			//Pula a linha no terminal

		value = ADC_to_Celsius(value);
		itoa(value,charValue,10);	//Converte "value" para a string chamada "charValue"
		UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
		UART_TX(" °C\r\n");  			//Pula a linha no terminal

		if(Read_P1(BUTTON_S3)) // A chave foi pressionada?
		{
			unsigned int i = 0, timeDelay;

			while(i < 3)
			{
				itoa(i,charValue,10);	//Converte "1" para a string chamada "charValue"
				UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
				UART_TX(":\t");  			//Pula a linha no terminal

				value = ADC_to_Celsius(ReadAdc()); //Lê a temperatura
				itoa(value,charValue,10);	//Converte "value" para a string chamada "charValue"
				UART_TX(charValue);			//Imprime na porta serial o valor em "charValue"
				UART_TX(" °C\r\n");  			//Pula a linha no terminal


				//Fase 1: Pré Aquecimento
				if(i == 0)
				{
					if(value >= 150)
					{
						i = 1;
						timeDelay = 180;
					}
					else OutPut_P1(LED_VERDE,1);
				}
				//Fase 2: Ramp Up
				if(i == 1)
				{
					if(timeDelay == 0)
					{
						i = 2;
						timeDelay = 10;
					}
					if(value <= 175) OutPut_P1(LED_VERDE,1);
					else OutPut_P1(LED_VERDE,0);
				}
				//Fase 3: Solda
				if(i == 2)
				{
					if(timeDelay == 0) i = 3;
					if(value <= 250) OutPut_P1(LED_VERDE,1);
					else OutPut_P1(LED_VERDE,0);
				}


				if (timeDelay > 0) timeDelay--; //Decrementa 1 segundo
				Toggle_P1(LED_2);
				__delay_cycles(1000000); 	//delay simples 1ms
			}
			OutPut_P1(LED_VERDE,0);






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
