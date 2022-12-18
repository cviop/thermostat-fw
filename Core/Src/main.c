#include "stm32f4xx.h"

#define setBit(reg, bit) ((reg) |= (1U << (bit)))
#define clrBit(reg, bit) ((reg) &= (~(1U << (bit))))
#define tglBit(reg, bit) ((reg) ^= (1U << (bit)))
#define readBit(reg, bit) ((reg) & (1<<bit))


void pwmSetup(void){
	setBit(RCC->APB1ENR, 0);        // Povoleni hodin casovace TIM2

	TIM2->PSC = 15;                    // preddelicka 16 na 1 MHz
	TIM2->ARR = 1000;                    // autoreload registr na 1000 Hz
	TIM2->CCR2 = 250;
	setBit(TIM2->EGR, 0);
}

void pinSetup(void){
	/*pwm pin PB3 setup*/
	setBit(TIM2->CCMR1, 14);            // PWM1 mode pro CC2
	setBit(TIM2->CCMR1, 13);            // PWM1 mode pro CC2
	setBit(TIM2->CCER, 4);            // Povoleni vystupu CC2

	setBit(RCC->AHB1ENR, 1);        // Povoleni hodin pro GPIOB


	setBit(GPIOB->MODER, 7);        // Nastaveni AF pro PB3
	clrBit(GPIOB->MODER, 6);        // Nastaveni AF pro PB3


	clrBit(GPIOB->PUPDR, 7);   // vypnuti pull-up/pull-down
	clrBit(GPIOB->PUPDR, 6);

	setBit(GPIOB->OSPEEDR, 6);    // nastaveni rychlosti brany na medium speed
	clrBit(GPIOB->OSPEEDR, 7);

	setBit(GPIOB->AFR[0], 12);        // Nastaveni AF1 pro pin PB3

	/*Analog in */

}



int main(void){

	pwmSetup();
	pinSetup();

    clrBit(TIM2->SR, 0);             // Obnoveni status registru po prerueseni
    setBit(TIM2->CR1, 0);            // Povoleni casovace TIM2 CEN
    setBit(RCC->CR, 0);             // Nastaveni HSI jako zroj hodin (16MHz)


    int i = 0;
    while(1){
    	/*
    	for(int k=0;k<10000;k++);
		TIM2->CCR2 = i++;
		if(i >1000)
			i = 0;*/


    }

}

