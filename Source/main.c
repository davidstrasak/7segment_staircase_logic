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

#define Number0 (A_seg | B_seg | C_seg | D_seg | E_seg | F_seg)
#define Number1 (B_seg | C_seg)
#define Number2 (A_seg | B_seg | D_seg | E_seg | G_seg)
#define Number3 (A_seg | B_seg | C_seg | D_seg | G_seg)
#define Number4 (B_seg | C_seg | F_seg | G_seg)
#define Number5 (A_seg | C_seg | D_seg | F_seg | G_seg)
#define Number6 (A_seg | C_seg | D_seg | E_seg | F_seg | G_seg)
#define Number7 (A_seg | B_seg | C_seg)
#define Number8 (A_seg | B_seg | C_seg | D_seg | E_seg | F_seg | G_seg)
#define Number9 (A_seg | B_seg | C_seg | D_seg | F_seg | G_seg)
#define Number_OFF 0x00

/*Globalni promenne*/


/*Funkcni prototypy*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(vu32 nCount);
void setNumber(int leftInput, int rightInput);
void removeFirst(void);

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
	
	GPIOA->BSRR|=(1 << (10));
	GPIOD->BSRR|=(1 << (2));
	
	while(counter < 15){
		// timhle bych dostal full sviticich ledek
		//GPIOC->BSRR|=(1<<(10 + 16));
		
		// timto bych mel dostat full nesviticich ledek
		GPIOC->BSRR|=(1<<(6));
		GPIOC->BSRR|=(1<<(7));
		GPIOB->BSRR|=(1<<(5));
		GPIOB->BSRR|=(1<<(6));
		
		GPIOC->BSRR|=(1 << (12));
		GPIOC->BSRR|=(1 << (12 + 16));

		counter++;
	}
	
	counter = 0;
	
	
	/*Nekonecna smycka*/
	while(1){
		GPIOA->BSRR|=(1 << (10+16));
		GPIOD->BSRR|=(1 << (2));
		setNumber(1,5);
		Delay(1000000);
		removeFirst();
		Delay(1000000);
		GPIOA->BSRR|=(1 << (10));
		GPIOD->BSRR|=(1 << (2+16));
		setNumber(8,8);
		Delay(1000000);
		removeFirst();
		Delay(1000000);
	}
}

void setNumber(int leftInput, int rightInput)
{
	int messageLeft;
	int messageRight;
	int counter = 0;
	int nextBitLeft;
	int nextBitRight;
	
		switch(leftInput){
		case 0:
			messageLeft = Number0;
			break;
		case 1:
			messageLeft = Number1;
			break;
		case 2:
			messageLeft = Number2;
			break;
		case 3:
			messageLeft = Number3;
			break;
		case 4:
			messageLeft = Number4;
			break;
		case 5:
			messageLeft = Number5;
			break;
		case 6:
			messageLeft = Number6;
			break;
		case 7:
			messageLeft = Number7;
			break;
		case 8:
			messageLeft = Number8;
			break;
		case 9:
			messageLeft = Number9;
			break;
		default:
			messageLeft = Number_OFF;
			break;
	}
	
	switch(rightInput){
		case 0:
			messageRight = Number0;
			break;
		case 1:
			messageRight = Number1;
			break;
		case 2:
			messageRight = Number2;
			break;
		case 3:
			messageRight = Number3;
			break;
		case 4:
			messageRight = Number4;
			break;
		case 5:
			messageRight = Number5;
			break;
		case 6:
			messageRight = Number6;
			break;
		case 7:
			messageRight = Number7;
			break;
		case 8:
			messageRight = Number8;
			break;
		case 9:
			messageRight = Number9;
			break;
		default:
			messageRight = Number_OFF;
			break;
	}
	
	messageLeft = messageLeft << 1;
	messageRight = messageRight << 1;
	
	GPIOC->BSRR|=(1 << (6));
	GPIOB->BSRR|=(1 << (5));

	while(counter != -1)
	{
		
		messageLeft = messageLeft >> 1;
		messageRight = messageRight >> 1;
		
		nextBitLeft = messageLeft & 1;
		nextBitRight = messageRight & 1;
		
		if(counter < 8){
			if(nextBitLeft == 1)
			{
				GPIOC->BSRR|=(1<<(7 + 16)); // obracena logika
			} 
			else
			{
				GPIOC->BSRR|=(1<<(7));
			}
			if(nextBitRight == 1)
			{
				GPIOB->BSRR|=(1<<(6 + 16)); // obracena logika
			}
			else
			{
				GPIOB->BSRR|=(1<<(6)); 
			}
		}
		else{
			counter = -1;
			continue;
		}
		
		GPIOC->BSRR|=(1 << (12));
		GPIOC->BSRR|=(1 << (12 + 16));
		counter++;
	}
	
	GPIOC->BSRR|=(1 << (6 + 16));
	GPIOB->BSRR|=(1 << (5 + 16));
}

void removeFirst(void) {
	
	int counter = 0;
	
	GPIOC->BSRR|=(1 << (6));
	GPIOB->BSRR|=(1 << (5));

	while(counter != -1)
	{		

			GPIOC->BSRR|=(1<<(7)); 
			GPIOB->BSRR|=(1<<(6)); 

		if(counter == 8){
			counter = -1;
			continue;
		}
		
		GPIOC->BSRR|=(1 << (12));
		GPIOC->BSRR|=(1 << (12 + 16));
		counter++;
	}
	
	GPIOC->BSRR|=(1 << (6 + 16));
	GPIOB->BSRR|=(1 << (5 + 16));

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

	RCC->APB2ENR|= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5); // Enable PA (2), PB (3), PC (4) and PD (5).
		
}
/*Inicializace GPIO*/
void GPIO_Configuration(void){
	
	// PA10 - Napajeni pro level displej
	GPIOA->CRH &= ~(0xF << 8);
	GPIOA->CRH |= (3 << 8);  // PP output
	// PB5 - DSA pro pravej displej
	GPIOB->CRL &= ~(0xF << 20);
	GPIOB->CRL |= (3 << 20);  // PP output
	// PB6 - DSA pro pravej displej
	GPIOB->CRL &= ~(0xF << 24);
	GPIOB->CRL |= (3 << 24);  // PP output
	// PC6 - DSA pro levej displej
	GPIOC->CRL &= ~(0xF << 24);
	GPIOC->CRL |= (3 << 24);  // PP output
	// PC7 - DSB pro level displej
	GPIOC->CRL &= 0x0FFFFFFF;
	GPIOC->CRL |= (3 << 28);  // PP output
	// PC12 - Clock
	GPIOC->CRH &= ~(0xF << 16);
	GPIOC->CRH |= (3 << 16);  // PP output
	// PD2 - Napajeni pro pravej displej
	GPIOD->CRL &= ~(0xF << 8);
	GPIOD->CRL |= (3 << 8);  // PP output

}

/*Delay smycka zpozduje zhruba o nCount tiku jadra*/
void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}
