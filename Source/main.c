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
#define A_seg  (1 << 7) 
#define B_seg  (1 << 6) 
#define C_seg  (1 << 5) 
#define D_seg  (1 << 4) 
#define E_seg  (1 << 3) 
#define F_seg  (1 << 2) 
#define G_seg  (1 << 1) 
#define DP_seg  (1 << 0) 

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
	int counter = 0;
	volatile uint32_t message = 0;
	volatile uint32_t nextBit;
	
	RCC_Configuration(); //inicializace hodin
	GPIO_Configuration(); //inicializace GPIO
	
	//GPIOB->BSRR|=(1 << (5 + 16));
	//GPIOB->BSRR|=(1 << (9 + 16));
	GPIOB->BSRR|=(1 << (5));
	GPIOB->BSRR|=(1 << (9));
	GPIOC->BSRR|=(1 << (11));
	GPIOC->BSRR|=(1 << (8));
	
	while(counter < 15){
		GPIOC->BSRR|=(1<<(10));
		
		GPIOA->BSRR|=(1 << (12));
		GPIOA->BSRR|=(1 << (12 + 16));

		counter++;
	}
	
	counter = 0;
	
	
	/*Nekonecna smycka*/
	while(1){
		if(counter == 0){
			message = (A_seg | B_seg | C_seg | E_seg | F_seg | G_seg) << 1;
		}
		
		message = message >> 1;
		
		nextBit = message & 1;
		
		if(counter < 8){
			volatile uint32_t temp = GPIOC->ODR;
			volatile uint32_t* temp_p = &GPIOC->ODR;
			temp &= ~(1 << 10);
			temp |= (~nextBit << 10);
			*temp_p = temp;
		}
		else if(counter < 16){
		GPIOC->BSRR|=(1<<(10));
		}
		else{
			counter = 0;
			continue;
		}
		
		GPIOA->BSRR|=(1 << (12));
		GPIOA->BSRR|=(1 << (12 + 16));
		counter++;
		if(counter < 8){
			Delay(100000);
		}
		else if (counter == 8){
			Delay(1000000);
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

	RCC->APB2ENR|=0x1C; // Enable PA (0x4), PB (0x8), and PC (0x10).
		
}
/*Inicializace GPIO*/
void GPIO_Configuration(void){
	GPIOA->CRH &= ~(0xF << 16);		// Vycistit bity 16-19 (pro PA12)
	GPIOA->CRH |= (3 << 16);	    // Nastavit PA12 jako PP output 50MHz
	
	GPIOC->CRH &= ~(0xF << 8);//PC10
	GPIOC->CRH |= (3 << 8);  //PC10 jako PP output
	
	GPIOC->CRH &= ~(0xF << 12);//PC11
	GPIOC->CRH |= (3 << 12);  //PC11 jako PP output
	
	GPIOB->CRL &= ~(0xF << 20);//PB5
	GPIOB->CRL |= (3 << 20);  //PB5 jako PP output
	
	GPIOB->CRH &= ~(0xF << 4);//PB9
	GPIOB->CRH |= (3 << 4);  //PB9 jako PP output
	
	GPIOC->CRH &= ~(0xF << 0);//PB8
	GPIOC->CRH |= (3 << 0);  //PB8 jako PP output
}

/*Delay smycka zpozduje zhruba o nCount tiku jadra*/
void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}
