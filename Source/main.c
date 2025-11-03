//;***************************************************************************************************
//;* CVUT FEL, Katedra Mereni
//;* Predmet: A4B38NVS
//;* Projekt: Schodistovy automat s 7-segmentovymi zobrazovaci
//;***************************************************************************************************

#include "stm32f10x.h"

/* Definice segmentu pro 7-segmentovy displej */
#define A_seg  (1 << 7) 
#define B_seg  (1 << 6) 
#define C_seg  (1 << 5) 
#define D_seg  (1 << 4) 
#define E_seg  (1 << 3) 
#define F_seg  (1 << 2) 
#define G_seg  (1 << 1) 
#define DP_seg (1 << 0) 

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

#define DEBOUNCE_COUNT 5
#define LONG_PRESS_TIME 50
#define MULTIPLEX_CYCLES_PER_SEC 50

#define STATE_SETUP 0
#define STATE_RUNNING 1

/* Funkcni prototypy */
void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(vu32 nCount);
void setNumber(int leftInput, int rightInput);
void clearDisplay(void);
void displayMultiplex(int leftNum1, int leftNum2, int rightNum1, int rightNum2);
int debounceButton(int currentState, int* counter, int* lastState);
void setLED(int state);
void displayTime(int seconds, int* digit1, int* digit2);

void SystemInit(void) {
}

int main(void) {
	int i;
	int debounceCountIncrease = 0, debounceStateIncrease = 1;
	int debounceCountDecrease = 0, debounceStateDecrease = 1;
	int debounceCountConfirm = 0, debounceStateConfirm = 1;
	int btnIncreaseRaw, btnDecreaseRaw, btnConfirmRaw;
	int btnIncrease, btnDecrease, btnConfirm;
	int prevBtnIncrease = 1, prevBtnDecrease = 1, prevBtnConfirm = 1;
	int pressTimeIncrease = 0, pressTimeDecrease = 0;
	int setTime = 10;
	int remainingTime = 10;
	int state = STATE_SETUP;
	int secondCounter = 0;
	int blinkCounter = 0;
	int ledState = 0;
	int leftDigit1, leftDigit2, rightDigit1, rightDigit2;

	RCC_Configuration();
	GPIO_Configuration();

	GPIOA->BSRR |= (1 << 10);
	GPIOD->BSRR |= (1 << 2);

	for (i = 0; i < 15; i++) {
		GPIOC->BSRR |= (1 << 6);
		GPIOC->BSRR |= (1 << 7);
		GPIOB->BSRR |= (1 << 5);
		GPIOB->BSRR |= (1 << 6);
		GPIOC->BSRR |= (1 << 12);
		GPIOC->BSRR |= (1 << (12 + 16));
	}

	while (1) {
		btnIncreaseRaw = (GPIOB->IDR & (1 << 7)) ? 1 : 0;
		btnDecreaseRaw = (GPIOB->IDR & (1 << 8)) ? 1 : 0;
		btnConfirmRaw = (GPIOB->IDR & (1 << 9)) ? 1 : 0;

		btnIncrease = debounceButton(btnIncreaseRaw, &debounceCountIncrease, &debounceStateIncrease);
		btnDecrease = debounceButton(btnDecreaseRaw, &debounceCountDecrease, &debounceStateDecrease);
		btnConfirm = debounceButton(btnConfirmRaw, &debounceCountConfirm, &debounceStateConfirm);

		if (state == STATE_SETUP) {
			if (btnIncrease == 0) {
				pressTimeIncrease++;

				if (pressTimeIncrease == LONG_PRESS_TIME) {
					setTime += 5;
					if (setTime > 99) setTime = 99;
				}
				else if (pressTimeIncrease > LONG_PRESS_TIME) {
					if ((pressTimeIncrease - LONG_PRESS_TIME) % MULTIPLEX_CYCLES_PER_SEC == 0) {
						setTime += 5;
						if (setTime > 99) setTime = 99;
					}
				}
			}
			else if (prevBtnIncrease == 0 && btnIncrease == 1) {
				if (pressTimeIncrease < LONG_PRESS_TIME) {
					setTime++;
					if (setTime > 99) setTime = 99;
				}
				pressTimeIncrease = 0;
			}

			if (btnDecrease == 0) {
				pressTimeDecrease++;

				if (pressTimeDecrease == LONG_PRESS_TIME) {
					setTime -= 5;
					if (setTime < 1) setTime = 1;
				}
				else if (pressTimeDecrease > LONG_PRESS_TIME) {
					if ((pressTimeDecrease - LONG_PRESS_TIME) % MULTIPLEX_CYCLES_PER_SEC == 0) {
						setTime -= 5;
						if (setTime < 1) setTime = 1;
					}
				}
			}
			else if (prevBtnDecrease == 0 && btnDecrease == 1) {
				if (pressTimeDecrease < LONG_PRESS_TIME) {
					setTime--;
					if (setTime < 1) setTime = 1;
				}
				pressTimeDecrease = 0;
			}

			if (btnConfirm == 0 && prevBtnConfirm == 1) {
				state = STATE_RUNNING;
				remainingTime = setTime;
				secondCounter = 0;
				blinkCounter = 0;
				setLED(1);
				ledState = 1;
			}

			remainingTime = setTime;
		}
		else {
			secondCounter++;
			if (secondCounter >= MULTIPLEX_CYCLES_PER_SEC / 2) {
				secondCounter = 0;
				blinkCounter++;

				if (blinkCounter >= 2) {
					blinkCounter = 0;
					remainingTime--;

					if (remainingTime <= 0) {
						remainingTime = setTime;
						state = STATE_SETUP;
						setLED(0);
						ledState = 0;
					}
				}

				if (remainingTime <= 3 && remainingTime > 0) {
					ledState = !ledState;
					setLED(ledState);
				}
			}

			if (btnConfirm == 0 && prevBtnConfirm == 1) {
				state = STATE_SETUP;
				remainingTime = setTime;
				blinkCounter = 0;
				setLED(0);
				ledState = 0;
			}
		}

		prevBtnIncrease = btnIncrease;
		prevBtnDecrease = btnDecrease;
		prevBtnConfirm = btnConfirm;

		displayTime(setTime, &leftDigit1, &leftDigit2);
		displayTime(remainingTime, &rightDigit1, &rightDigit2);
		displayMultiplex(leftDigit1, leftDigit2, rightDigit1, rightDigit2);
	}
}

void setNumber(int leftInput, int rightInput)
{
	int messageLeft;
	int messageRight;
	int counter = 0;
	int nextBitLeft;
	int nextBitRight;

	switch (leftInput) {
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

	switch (rightInput) {
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

	GPIOC->BSRR |= (1 << (6));
	GPIOB->BSRR |= (1 << (5));

	while (counter != -1)
	{

		messageLeft = messageLeft >> 1;
		messageRight = messageRight >> 1;

		nextBitLeft = messageLeft & 1;
		nextBitRight = messageRight & 1;

		if (counter < 8) {
			if (nextBitLeft == 1)
			{
				GPIOC->BSRR |= (1 << (7 + 16)); // obracena logika
			}
			else
			{
				GPIOC->BSRR |= (1 << (7));
			}
			if (nextBitRight == 1)
			{
				GPIOB->BSRR |= (1 << (6 + 16)); // obracena logika
			}
			else
			{
				GPIOB->BSRR |= (1 << (6));
			}
		}
		else {
			counter = -1;
			continue;
		}

		GPIOC->BSRR |= (1 << (12));
		GPIOC->BSRR |= (1 << (12 + 16));
		counter++;
	}

	GPIOC->BSRR |= (1 << (6 + 16));
	GPIOB->BSRR |= (1 << (5 + 16));
}

void clearDisplay(void) {
	int i;
	GPIOC->BSRR |= (1 << 6);
	GPIOB->BSRR |= (1 << 5);

	for (i = 0; i < 8; i++) {
		GPIOC->BSRR |= (1 << 7);
		GPIOB->BSRR |= (1 << 6);
		GPIOC->BSRR |= (1 << 12);
		GPIOC->BSRR |= (1 << (12 + 16));
	}

	GPIOC->BSRR |= (1 << (6 + 16));
	GPIOB->BSRR |= (1 << (5 + 16));
}

int debounceButton(int currentState, int* counter, int* lastState) {
	if (currentState == *lastState) {
		*counter = 0;
	}
	else {
		(*counter)++;
		if (*counter >= DEBOUNCE_COUNT) {
			*lastState = currentState;
			*counter = 0;
		}
	}
	return *lastState;
}

void setLED(int state) {
	if (state) {
		GPIOC->BSRR |= (1 << 8);
	}
	else {
		GPIOC->BSRR |= (1 << (8 + 16));
	}
}

void displayTime(int seconds, int* digit1, int* digit2) {
	*digit1 = seconds / 10;
	*digit2 = seconds % 10;
}

void displayMultiplex(int leftNum1, int leftNum2, int rightNum1, int rightNum2) {
	// Zobrazeni praveho bloku (PD2 aktivni)
	GPIOA->BSRR |= (1 << 10);         // PA10 high - vypni levy blok
	GPIOD->BSRR |= (1 << (2 + 16));   // PD2 low - zapni pravy blok
	setNumber(leftNum1, rightNum1);
	Delay(10);
	clearDisplay();

	// Zobrazeni leveho bloku (PA10 aktivni)
	GPIOA->BSRR |= (1 << (10 + 16));  // PA10 low - zapni levy blok
	GPIOD->BSRR |= (1 << 2);          // PD2 high - vypni pravy blok
	setNumber(leftNum2, rightNum2);
	Delay(10);
	clearDisplay();
}

/*Inicializace RCC*/
void RCC_Configuration(void) {
	RCC->CR |= 0x10000; //HSE on
	while (!(RCC->CR & 0x20000)) {}
	//flash access setup
	FLASH->ACR &= 0x00000038;   //mask register
	FLASH->ACR |= 0x00000002;   //flash 2 wait state

	FLASH->ACR &= 0xFFFFFFEF;   //mask register
	FLASH->ACR |= 0x00000010;   //enable Prefetch Buffer

	RCC->CFGR &= 0xFFC3FFFF;//maskovani PLLMUL
	RCC->CFGR |= 0x1 << 18;//Nastveni PLLMUL 3x
	RCC->CFGR |= 0x0 << 17;//nastaveni PREDIV1 1x
	RCC->CFGR |= 0x10000;//PLL bude clocovan z PREDIV1
	RCC->CFGR &= 0xFFFFFF0F;//HPRE=1x
	RCC->CFGR &= 0xFFFFF8FF;//PPRE2=1x
	RCC->CFGR &= 0xFFFFC7FF;//PPRE2=1x

	RCC->CR |= 0x01000000;//PLL on
	while (!(RCC->CR & 0x02000000)) {}//PLL stable??

	RCC->CFGR &= 0xFFFFFFFC;
	RCC->CFGR |= 0x2;//nastaveni PLL jako zdroj hodin pro SYSCLK

	while (!(RCC->CFGR & 0x00000008))//je SYSCLK nastaveno?
	{
	}

	RCC->APB2ENR |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
}

/*Inicializace GPIO*/
void GPIO_Configuration(void) {
	// PA10 - Napajeni pro level displej
	GPIOA->CRH &= ~(0xF << 8);
	GPIOA->CRH |= (3 << 8);  // PP output
	// PB5 - DSA pro pravej displej
	GPIOB->CRL &= ~(0xF << 20);
	GPIOB->CRL |= (3 << 20);  // PP output
	// PB6 - DSB pro pravej displej
	GPIOB->CRL &= ~(0xF << 24);
	GPIOB->CRL |= (3 << 24);  // PP output
	// PB7 - Tlacitko INCREASE (s externim pull-up)
	GPIOB->CRL &= 0x0FFFFFFF;
	GPIOB->CRL |= (0x4 << 28);  // Floating input
	// PB8 - Tlacitko DECREASE (s externim pull-up)
	GPIOB->CRH &= ~(0xF << 0);
	GPIOB->CRH |= (0x4 << 0);  // Floating input
	// PB9 - Tlacitko CONFIRM (s externim pull-up)
	GPIOB->CRH &= ~(0xF << 4);
	GPIOB->CRH |= (0x4 << 4);  // Floating input
	// PC6 - DSA pro levy displej
	GPIOC->CRL &= ~(0xF << 24);
	GPIOC->CRL |= (3 << 24);  // PP output
	// PC7 - DSB pro levy displej
	GPIOC->CRL &= 0x0FFFFFFF;
	GPIOC->CRL |= (3 << 28);  // PP output
	// PC8 - LED pro simulaci rele
	GPIOC->CRH &= ~(0xF << 0);
	GPIOC->CRH |= (3 << 0);  // PP output
	// PC12 - Clock
	GPIOC->CRH &= ~(0xF << 16);
	GPIOC->CRH |= (3 << 16);  // PP output
	// PD2 - Napajeni pro pravej displej
	GPIOD->CRL &= ~(0xF << 8);
	GPIOD->CRL |= (3 << 8);  // PP output
}

/*Delay smycka zpozduje zhruba o nCount milisekund*/
void Delay(vu32 nCount)
{
	int i;
	for (; nCount != 0; nCount--) {
		for (i = 6000; i != 0; i--);
	}
}
