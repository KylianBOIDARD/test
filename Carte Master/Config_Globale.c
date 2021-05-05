//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER CONFIGURATION GLOBALE
//
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include <C8051F020.h>
#include "c8051F020_SFR16.h"
void Init_Device(void) ;

//-----------------------------------------------------------------------------
// Dévalidation du watchdog
//-----------------------------------------------------------------------------
void Reset_Sources_Init()
{
	 WDTCN = 0xDE;
	 WDTCN = 0XAD;
}

//-----------------------------------------------------------------------------
// Config oscillateur - SYSCLK = 22,1184MHz - Oscillateur externe à quartz 
//-----------------------------------------------------------------------------
void Oscillator_Init_Quartz()
{

	  int i = 0;
    OSCXCN    = 0x67;  								// Config de l'horloge externe - Quartz > 6,7 MHz
    for (i = 0; i < 3000; i++);  			// attente stabilisation Fosc quartz
    while ((OSCXCN & 0x80) == 0); 		// validation stabilité du quartz
    OSCICN = 0x88;  									// Commutation sur oscillateur externe 
	        
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Configuration des Ports d'entrée-sorties
//-----------------------------------------------------------------------------

void Port_IO_Init()
{
//------ FOM1 -----------------
    // P0.0  -  TX0, Push-Pull, Digital
    // P0.1  -  RX0, Open-Drain, Digital
		P0MDOUT   |=  0x01 ;
		XBR0      |= 0x04 ;		//uart0 enable
//------ FOM2 -------------------
    // P0.2  -  TX1, Push-Pull, Digital
    // P0.3  -  RX1,  Open-Drain, Digital
		XBR2 |= 0x44 ;			//uart1 enable
		P0MDOUT |= (1<<2) ;
//------- FOM3 ---------------
    // P0.4  -  Servo Horizontal,  Push-Pull, Digital
    P0MDOUT |= (1<<4) ; // set P0.4 output state to push-pull
//-------- FOM4 ----------------
    // P0.5  -  TriggTélémètreAV,  Push-Pull, Digital
    // P0.6  -  TriggTélémètreAR,  Push-Pull, Digital
		// P3.6  -  ECHO_AV,  Open-Drain, Digital Input INT6
    // P3.7  -  ECHO_AR,  Open-Drain, Digital Input INT7
		P0MDOUT |= 0x60 ;			

    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital
    // P3.1  -  Unassigned,  Open-Drain, Digital
    // P3.2  -  Unassigned,  Open-Drain, Digital
    // P3.3  -  Unassigned,  Open-Drain, Digital
    // P3.4  -  Unassigned,  Open-Drain, Digital
    // P3.5  -  Unassigned,  Open-Drain, Digital

		
		// P4.0 to P7.7   Unassigned,  Open-Drain, Digital
		
		// P4.0  -  RTS, , Digital  
		// P4.1  -  CTS, , Digital
		// Il reste a router GND, RX1, TX1 de l'uart1 sur le crossbar master selon les priorités

}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//------- CONFIGURATION DE LA COMMUNICATION SPI ------------------------------------

void Config_SPI(void){
		SPIEN = 1 ;		//active la com SPI
		MSTEN = 1 ; 	//mode master 
		SPI0CKR = 4 ;	//réglage de SCK à SYSCLK/2*(4+1)= 2.21184 MHz

}

void Init_T3(void){
		TMR3CN |= (1<<2) ;
		TMR3 = 0xCC ;
}

void Delay_1ms(void){
		int temp = TMR3 ;
		while( (TMR3-temp) < 1851 ){}
}

void Delay(const int time_wait) {
	int i;
	for (i = 0; i < time_wait; i++)	{
		Delay_1ms();
	}
}


//----------------------------------------------------------------------------------
void Init_Device(void){
		Reset_Sources_Init();
		Oscillator_Init_Quartz();
		XBR2 |= 0x40 ;   							//crossbar enable
    Port_IO_Init();
		EA = 1 ;											//enable all interrupts
		Config_SPI();
		Init_T3();
}









