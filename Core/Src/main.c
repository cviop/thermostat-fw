#include "stm32f4xx.h"

#define setBit(reg, bit) ((reg) |= (1U << (bit)))
#define clrBit(reg, bit) ((reg) &= (~(1U << (bit))))
#define tglBit(reg, bit) ((reg) ^= (1U << (bit)))
#define getBit(reg, bit) ((reg) & (1<<bit))

uint16_t val=0;

uint16_t val1=0;

uint16_t val_noint = 0;
/*
void ADC_IRQHandler(){
	if(getBit(ADC1->SR, 1)){
		val = ADC1->DR;
		val1=val;
	}
}
*/

void pwmSetup(void){
	setBit(RCC->APB1ENR, 0);        // Povoleni hodin casovace TIM2

	TIM2->PSC = 15;                    // preddelicka 16 na 1 MHz
	TIM2->ARR = 4096;                    // autoreload registr na 1000 Hz
	TIM2->CCR2 = 250;
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

	clrBit(ADC1->CR1, 24);     	// 12-bit resolution (Tconv = 15 ADCCLK cycles)
	clrBit(ADC1->CR1, 25);    	// reset state

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

    clrBit(TIM2->SR, 0);             // Obnoveni status registru po prerueseni
    setBit(TIM2->CR1, 0);            // Povoleni casovace TIM2 CEN
    setBit(RCC->CR, 0);             // Nastaveni HSI jako zroj hodin (16MHz)


    while(1){



    	setBit(ADC1->CR2, 30);             // software ADC start
		while (!getBit(ADC1->SR, 1));     // wait until conversion end
		val_noint= ADC1->DR;
		TIM2->CCR2 = val_noint;
		delay(10);

    }

}

