#include "stm32f4xx.h"

#define setBit(reg, bit) ((reg) |= (1U << (bit)))
#define clrBit(reg, bit) ((reg) &= (~(1U << (bit))))
#define tglBit(reg, bit) ((reg) ^= (1U << (bit)))
#define getBit(reg, bit) ((reg) & (1<<bit))

uint16_t val=0;

uint16_t val1=0;

uint8_t val_noint = 0;
/*
void ADC_IRQHandler(){
	if(getBit(ADC1->SR, 1)){
		val = ADC1->DR;
		val1=val;
	}
}
*/

void sendChar (int ch)  {
  while (!getBit(USART2->SR,7));
  USART2->DR = ch;
}

void serial2Init(void){        // USART2 je pripojeny k USB
  setBit(RCC->APB1ENR, 17);    // Povoleni hodin na UART2
  setBit(USART2->CR1, 13);    // Povoleni rozhrani USART2
  setBit(USART2->CR1, 2);    // Povoleni prijmu
  setBit(USART2->CR1, 3);    // Povoleni odesilani
  USART2->BRR = (11<<0) | (8<<4); // Nastaveni rychlosti na ?

  setBit(RCC->AHB1ENR, 0);    // nastaveni hodin portu A
  setBit(GPIOA->MODER, 5);    // Alternativni funkce pro PA.2
  setBit(GPIOA->MODER, 7);    // Alternativni funkce pro PA.3

  GPIOA->AFR[0]  |= (7<<8);   // Bits (11:10:9:8) = 0:1:1:1  --> AF7 Alternate function for USART2 at Pin PA2
  GPIOA->AFR[0]  |= (7<<12);   // Bits (15:14:13:12) = 0:1:1:1  --> AF7 Alternate function for USART2 at Pin PA3
}

uint8_t UART2_getchar(void){
    uint8_t temp;
    while(!(USART2->SR & (1<<5)));
    temp = USART2->DR;
    return temp;
}

void pwmSetup(void){
	setBit(RCC->APB1ENR, 0);        // Povoleni hodin casovace TIM2

	TIM2->PSC = 15;                    // preddelicka 16 na 1 MHz
	TIM2->ARR = 100;                    // autoreload registr na 1000 Hz
	TIM2->CCR2 = 50;
	setBit(TIM2->EGR, 0);

	setBit(TIM2->CCMR1, 14);            // PWM1 mode pro CC2
	setBit(TIM2->CCMR1, 13);            // PWM1 mode pro CC2
	setBit(TIM2->CCER, 4);           	// Povoleni vystupu CC2
}

void pinSetup(void){
	/*pwm pin PB3 setup*/


	setBit(RCC->AHB1ENR, 1);        // Povoleni hodin pro GPIOB


	setBit(GPIOB->MODER, 7);        // Nastaveni AF pro PB3
	clrBit(GPIOB->MODER, 6);        // Nastaveni AF pro PB3


	clrBit(GPIOB->PUPDR, 7);   // vypnuti pull-up/pull-down
	clrBit(GPIOB->PUPDR, 6);

	setBit(GPIOB->OSPEEDR, 6);    // nastaveni rychlosti brany na medium speed
	clrBit(GPIOB->OSPEEDR, 7);

	setBit(GPIOB->AFR[0], 12);        // Nastaveni AF1 pro pin PB3

	/*Analog in on PA0*/
	setBit(RCC->AHB1ENR, 0);        // Povoleni hodin pro GPIOA

	setBit(GPIOA->MODER, 0);		//analog mode for pin PA0
	setBit(GPIOA->MODER, 1);

	clrBit(GPIOA->PUPDR, 0);		//No Pu-Pd on PA0
	clrBit(GPIOA->PUPDR, 1);


}

void adcSetup(){

	// initialize the HSI clock
	setBit(RCC->CR, 0);         // enable HSI
	while (!getBit(RCC->CR, 1));// wait until HSI stable

	setBit(RCC->APB2ENR, 8);     // enable ADC1 peripheral clock

	//ADC->CCR = 0;

	//setBit(ADC1->CR1, 5);		//Interrupt enable for EOC
	//NVIC_EnableIRQ(ADC_IRQn);	//mask ADC irq

	/*set 480 cycles for ADC*/
	setBit(ADC1->SMPR2, 0);
	setBit(ADC1->SMPR2, 1);
	setBit(ADC1->SMPR2, 2);

	clrBit(ADC1->SQR3, 0); 		//set first and only channel in seq. as ADC1_ch0
	clrBit(ADC1->SQR3, 1);   	// reset state - all conversions from channel0 (PA_0)

	//clrBit(ADC1->CR1, 24);     	// 12-bit resolution (Tconv = 15 ADCCLK cycles)
	//clrBit(ADC1->CR1, 25);    	// reset state

    setBit(ADC1->CR1, 25);         // 8-bit resolution (Tconv =  11 ADCCLK cycles)


	//setBit(ADC1->CR2, 1);		//cont. mode;
	setBit(ADC1->CR2, 0);		// turn on ADC


}
void delay(int x)
{
    volatile int i;
    for (i = 0; i < 1000*x; i++);
}


int main(void){

	pwmSetup();
	pinSetup();
	adcSetup();
	serial2Init();

    clrBit(TIM2->SR, 0);             // Obnoveni status registru po prerueseni
    setBit(TIM2->CR1, 0);            // Povoleni casovace TIM2 CEN
    setBit(RCC->CR, 0);             // Nastaveni HSI jako zroj hodin (16MHz)

    uint8_t msg = 0;

    while(1){
    	msg=UART2_getchar();
    if(msg>100){
    	setBit(ADC1->CR2, 30);             // software ADC start
		while (!getBit(ADC1->SR, 1));     // wait until conversion end
		msg= ADC1->DR;
		sendChar(msg);
    }
    else{
    	TIM2->CCR2 = msg;
    }

		/*
    	setBit(ADC1->CR2, 30);             // software ADC start
		while (!getBit(ADC1->SR, 1));     // wait until conversion end
		val_noint= ADC1->DR;
		TIM2->CCR2 = val_noint;
		delay(10);
		*/

    }
}

