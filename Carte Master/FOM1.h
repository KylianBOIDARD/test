#ifndef __FOM1_H__
#define __FOM1_H__
#include <decodage.h>
#include <encodage.h>
#include <FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h>


#define SYSCLK 22118400 //approximate SYSCLK frequency in Hz
#define BAUDRATE  19200L          // Baud rate of UART in bps
                                   // Le caractère 'L' force l'évaluation de BAUDRATE en entier long
#define TICK_CLK  200  // (en hertz)



void Init_TIMER1();
void Reset_Sources_Init();
void Port_IO_Init();
void Oscillator_Init_Osc_Quartz();
void Init_UART0();
void Init_FOM1 (void);


void Interruption_UART();

void init_Serial_Buffer(void);
unsigned char serOutchar(char c);
char serInchar(void); 
unsigned int serInchar_Bin(void);
unsigned char serOutstring(char *buf);
void cfg_Clock_UART(void);
void cfg_UART0_mode1(void);
	

void Phase_Demarrage(void);
void FOM1(void);

void FO_M6(void);
#endif





