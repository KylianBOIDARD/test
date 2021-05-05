//-----------------------------------------------------------------------------
// FF-S1.c
//-----------------------------------------------------------------------------
// AUTH: LAKKIS - DUCOURAU
// DATE: 02/03/2021
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include "FF_S1.h"
//------ VARIABLES GLOBALES --------------------------------------------------
sbit LUM = P1^2;

static unsigned char Intensite;
static unsigned char Lum_Nbre;
static int compteur_cycle;
static int LED_ON;
static int LED_OFF;
static int cycle_haut;
static int cycle_bas;
static unsigned char Lum_ON;
static unsigned char Lum_OFF;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
/*
void main (void) {    //il faut creer une fct FF_S4() 
       
		initialisation();
		T4CON |= 0x04;
	
	while(1){}				               	
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Fonctions de configuration des divers périphériques
//-----------------------------------------------------------------------------
// Insérez vos fonctions de configuration ici

void init_FFS1 (void){
		// Crossbar
		/*XBR2 |= 0x40;
		XBR1 |= 0x80;
	
		// Sorties
		P1MDOUT |= 0x40;*/
	
		// Interrupt 
		EIE2 |= 0x04;
		EIE2 |= 0x20;
	
		// Config 
		config_clock();
		config_timer4();

// Paramètres d'entrée à changer____________________________________________________________________
		Intensite=60;
		Lum_ON=15;
	  Lum_OFF=25;
		Lum_Nbre=10;
		compteur_cycle=0;
		return;
// __________________________________________________________________________________________________
}

void config_clock (void){
		unsigned int n;
		OSCXCN = 0x67;
		for (n=0;n<2;n++){
			Software_Delay_1ms();
		}
		OSCICN |= 0x08;
		return;
}

void config_timer4(void){
		T4CON |= 0x01;
		T4CON &= 0xFD;
		RCAP4L = 0xFF;
		RCAP4H = 0xEF;
		CKCON |= 0x40;
		T4CON &= 0x7F;
		return;
}


void Lumiere (unsigned char Intensite, unsigned char Lum_ON, unsigned char Lum_OFF,
unsigned char Lum_Nbre){
	
	if (compteur_cycle <= Lum_Nbre -1){
			if (LED_ON <= Lum_ON*37.5){
				LED_ON++;
				allumage_MLI(Intensite);
			}
			else{
				if (LED_OFF <= Lum_OFF*37.5){
					LED_OFF++;
					LUM=0;
				}
				else{ 	// reset des variables
					LED_ON=0;
					LED_OFF=0;
					compteur_cycle++;
				}
			}
	
	}
	
	else{
		T4CON &= 0xFB; //arrêt du timer 4
		compteur_cycle=0;
	}
	return;
}	
	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Fonctions d'interruptions
//-----------------------------------------------------------------------------
// Insérez vos fonctions d'interruption ici

void timer_interrupt(void) interrupt 16 {
	Lumiere(Intensite, Lum_ON, Lum_OFF, Lum_Nbre);
	T4CON &= 0x7F; //On baisse le flag
}

void super_extinction(void) interrupt 19{
	LUM=0;
	T4CON &= 0xFB;
}

	
// Fonction MLI ______________________________________________________________

void allumage_MLI (unsigned int alpha){
	if (cycle_haut*20<alpha){
		cycle_haut++;
		LUM=1;
		cycle_bas=cycle_haut;
	}
	else{
		if (cycle_bas*20<100){
			LUM=0;
			cycle_bas++;
		}
		else {
			cycle_haut=0;
			cycle_bas=0;
		}
	}
	return;
}
// Delay de 1ms _____________________________________

void Software_Delay_1ms(void)
   { 
	 unsigned int i;
	 for(i=0;i<99;i++){}
	 }
	 