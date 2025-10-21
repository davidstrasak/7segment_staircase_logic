//;***************************************************************************************************
//;*
//;* Misto			: CVUT FEL, Katedra Mereni
//;* Prednasejici		: Doc. Ing. Jan Fischer,CSc.
//;* Predmet			: A4B38NVS
//;* Vyvojovy Kit		: STM32 VL DISCOVERY (STM32F100RB)
//;*
//;**************************************************************************************************
//;*
//;* JMÉNO PROJEKTU	: Demo_01
//;* AUTOR			: Radek Øípa
//;* DATUM			: 11/2011
//;* POPIS			: Program pro ovladani LED na vyvodu PC8 pomoci USER tlacitka na PA0.
//;*					  - konfigurace hodin na frekvenci 24MHz (HSE + PLL) 
//;*					  - konfigurace pouzitych vyvodu procesotu (PC8 jako push-pull vystup a tlacitka PA0 jako floating input)
//;*					 
//;*
//;***************************************************************************************************

#include  "stm32f10x.h"


/*Definice*/

/*Globalni promenne*/


/*Funkcni prototypy*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(vu32 nCount);

/*Metody*/


/*System init
 *Volano z startup souboru
 *Metoda je prazdna aby se mohla provest inicializace v metode main.
 *Jinak se inicializace provadi v souboru system_stm32f10x.h

*/
void SystemInit(void){
//Prazdna inicializacni metoda	
}

/*Main funkce*/
int main(void){
	RCC_Configuration(); //inicializace hodin
	GPIO_Configuration(); //inicializace GPIO
	/*Nekonecna smycka*/
	while(1){
		if(GPIOA->IDR&0x1){//je PA0 stisknuto??
			GPIOC->BSRR|=0x100;//ANO rozsvitime LED na PC8	
			GPIOA->BSRR|=(1<<(12+16));
		}else{
			GPIOC->BSRR|=0x1000000;//ne zhasneme LED na PC8
			GPIOA->BSRR|=(1<<12);
		}

	}
}
/*Inicializace RCC*/
void RCC_Configuration(void){
	RCC->CR|=0x10000; //HSE on
	while(!(RCC->CR&0x20000)){}
	  //flash access setup
  	FLASH->ACR &= 0x00000038;   //mask register
  	FLASH->ACR |= 0x00000002;   //flash 2 wait state

 	FLASH->ACR &= 0xFFFFFFEF;   //mask register
    FLASH->ACR |= 0x00000010;   //enable Prefetch Buffer

	RCC->CFGR&=0xFFC3FFFF;//maskovani PLLMUL
	RCC->CFGR|=0x1<<18;//Nastveni PLLMUL 3x
	RCC->CFGR|=0x0<<17;//nastaveni PREDIV1 1x
	RCC->CFGR|=0x10000;//PLL bude clocovan z PREDIV1
	RCC->CFGR&=0xFFFFFF0F;//HPRE=1x
	RCC->CFGR&=0xFFFFF8FF;//PPRE2=1x
	RCC->CFGR&=0xFFFFC7FF;//PPRE2=1x

	RCC->CR|=0x01000000;//PLL on
	while(!(RCC->CR&0x02000000)){}//PLL stable??

	RCC->CFGR&=0xFFFFFFFC;
	RCC->CFGR|=0x2;//nastaveni PLL jako zdroj hodin pro SYSCLK

  	while(!(RCC->CFGR & 0x00000008))//je SYSCLK nastaveno?
  	{
  	}

	RCC->APB2ENR|=0x14;//pocoleni PA a PC
		
}
/*Inicializace GPIO*/
void GPIO_Configuration(void){
	GPIOC->CRH&= ~(0xF << 0);//PC8
	GPIOC->CRH|=0x3;  //PC8 jako PP output

	GPIOA->CRL&= ~(0xF << 0);
	GPIOA->CRL|=0x4;//PA0 jako Floating input
	
	GPIOA->CRH &= ~(0xF << 16);		// Vycistit bity 16-19 (pro PA12)
	GPIOA->CRH |= (3 << 16);	    // Nastavit PA12 jako PP output 50MHz
}

/*Delay smycka zpozduje zhruba o nCount tiku jadra*/
void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}
