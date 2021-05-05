//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER SOURCE DU SOUS ENSEMBLE FO-M4
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
//-------- Utilisation du télémètre Ultrasons ---------------------------------
#include "FO-M4.h"

//------------------------------------------------------------------------------
//---------------- Initialisation des variables globales -----------------------
//------------------------------------------------------------------------------
sbit  TRIGGER_AV = P0^5 ; //Télémètre Avant Commande
sbit	ECHO_AV = P3^6 ;		//Réponse AV
sbit  TRIGGER_AR = P0^6 ; //Télémètre Arrière
sbit	ECHO_AR = P3^7 ;		//Réponse AR

void Delay(const int time_wait);

char attente_mesure = 0 ;
int temps_mesure = 0 ;
char overflow = 0 ;
unsigned int distance ;
unsigned int i_m4 ;

/*
unsigned int   limit;
unsigned int  counter_ISR = 0;
unsigned int   last_counter = 0;
unsigned int   distance_AV;
unsigned int   distance_AR;
unsigned int   AR;
unsigned int   AV;*/


//------------------------------------------------------------------------------
//---------------- Fonction d'Initialisation de FO-M4 --------------------------
//------------------------------------------------------------------------------
void Init_FO_M4(void){
	EIE2|=0x04; 			//enable T4 interruption
	EIE2 |= (1<<4) ;  //enable intext6
	P3IF |= (1<<2) ;	//déclenche sur front montant
	EIE2 |= (1<<5) ;  //enable intext7
	P3IF |= (1<<3) ;	//déclenche sur front montant
	TRIGGER_AV = 0 ;	//initialise les signaux
	TRIGGER_AR = 0 ;	//de commande des télémètres 

}

//------------------------------------------------------------------------------
//---------------- Fonction Principale de FO-M4 --------------------------------
//------------------------------------------------------------------------------
int FO_M4(char ordre){
	if (ordre ==1){
		MES_Dist_AV();
	}
	if (ordre ==2){
		MES_Dist_AR();
	}
	return distance;
}

//------------------------------------------------------------------------------
//---------------- Fonction de mesure de distance AVANT ------------------------
//------------------------------------------------------------------------------
void MES_Dist_AV (void) {
	T4CON = 0x00 ;
	TH4 = 0 ; TL4 = 0 ;						//reset du timer4
	attente_mesure = 0 ;
	overflow = 0 ;
	Trigger_AV();									//on demande la detection
	while (attente_mesure == 0); 	//on attend que la reponse arrive
	attente_mesure = 0 ;					//on est rentré dans l'int donc la mesure a commencée
	while (ECHO_AV != 0); 					//tant qu'on est encore dans la mesure
	T4CON = 0x00 ; 								//on coupe le T4
	temps_mesure = (TH4<<8) + TL4 ;  		//on recupere le temps compté
	if( overflow == 1 || temps_mesure > 0xB4D8 ){	//si t4 overflow, donc que temp > 35ms ou que temps > 25ms
		//la deuxieme condition est une sécurité, normalement c'est 25ms max ou alors 38ms
			distance = 0 ;
	}
	else{
			distance = convert(temps_mesure) ; 	//on converti en distance cm
	}
}

//------------------------------------------------------------------------------
//---------------- Fonction de mesure de distance ARRIERE ----------------------
//------------------------------------------------------------------------------
void MES_Dist_AR (void) {
	T4CON = 0x00 ;
	TH4 = 0 ; TL4 = 0 ;						//reset du timer4
	attente_mesure = 0 ;
	overflow = 0 ;
	Trigger_AR();
	while (attente_mesure == 0); 	//on attend que la reponse arrive
	attente_mesure = 0 ;					//on est rentré dans l'int donc la mesure a commencée
	while (ECHO_AR != 0); 				//tant qu'on est encore dans la mesure
	T4CON = 0x00 ; 								//on coupe le T4
	temps_mesure = (TH4<<8) + TL4 ;  		//on recupere le temps compté
	if( overflow == 1 || temps_mesure > 0xB4D8 ){	//si t4 overflow, donc que temp > 35ms ou que temps > 25ms
		//la deuxieme condition est une sécurité, normalement c'est 25ms max ou alors 38ms
			distance = 0 ;
	}
	else{
			distance = convert(temps_mesure) ; 	//on converti en distance cm
	}
}

//------------------------------------------------------------------------------
//---------------- Fonctions Interruptions -------------------------------------
//------------------------------------------------------------------------------
void EXT6_int(void) interrupt 18{
		T4CON = 0x04 ;				//on active Timer4 pour compter le temps a l'etat HAUT
		P3IF &= ~(1<<6) ;			//remise a zero du flag
		attente_mesure = 1 ;	//on indique qu'on a commencé a compter	
}

void EXT7_int(void) interrupt 19{
		T4CON = 0x04 ;				//on active Timer4 pour compter le temps a l'etat HAUT
		P3IF &= ~(1<<7) ;			//remise a zero du flag
		attente_mesure = 1 ;	//on indique qu'on a commencé a compter	
}

void Timer4_int(void) interrupt 16{
		T4CON &= ~(1<<7) ;	//remise à zéro du flag
		overflow = 1 ;
}

//------------------------------------------------------------------------------
//-------- Fonction Génératrice de Signaux de Commande pour télémetre ----------
//------------------------------------------------------------------------------
void Trigger_AV (void) {
	TRIGGER_AV = 1;
	Delay(1);
	TRIGGER_AV = 0;
}
void Trigger_AR (void) {
	TRIGGER_AR = 1;
	Delay(1);
	TRIGGER_AR = 0;
}
//------------------------------------------------------------------------------
//---------------- Fonction Conversion Temps/Distance --------------------------
//------------------------------------------------------------------------------
unsigned int convert (int x) {
	unsigned int dist = 0;
	//le timer s'incrémente toutes les 0.54us
	//Formule : distance (cm) = time(us)/58
	dist = (x*0.54)/58;
	return dist ;
}



//------------------------------------------------------------------------------
//---------------- Ancienne Version de FO-M4 --------------------------------
//------------------------------------------------------------------------------

/*Fonction appelee par le Time4 qui compte la duree d'impulsion a l'etat haut
	Stock dans last_counter la valeur du compteur (tick)*/
/*
void Timer4_ISR (void) interrupt 16 {
	TIMER4=1;
	if (AR == 1) {
		//Si l'echo est a zero et que le compteur etait aussi a zero, on met quand meme a zero
		if (ECHO2 == 0 && counter_ISR == 0) {
			counter_ISR = 0;
		}
		//Si l'echo est nul et que le counter n'est pas nul, on vient d'avoir un front descendant, on stock la variable count_ISR
		//On lance la conversion et on desactive le timer4
		else if (ECHO2 == 0 && counter_ISR != 0) {
			last_counter = counter_ISR;
			counter_ISR = 0;
			T4CON |= ~(0x04);
			distance_AR = convert();
		}
		//Si l'echo est a 1 on increment le counter
		else {
			counter_ISR ++;
		}
	}
	
		if (AV == 1) {
		//Si l'echo est a zero et que le compteur etait aussi a zero, on met quand meme a zero
		if (ECHO1 == 0 && counter_ISR == 0) {
			counter_ISR = 0;
		}
		//Si l'echo est nul et que le counter n'est pas nul, on vient d'avoir un front descendant, on stock la variable count_ISR
		//On lance la conversion et on desactive le timer4
		if (ECHO1 == 0 && counter_ISR != 0) {
			last_counter = counter_ISR;
			counter_ISR = 0;
			T4CON &= ~(0x04);
			distance_AV = convert();
		}
		//Si l'echo est a 1 on increment le counter
		if (ECHO1 == 1) {
			counter_ISR ++;
		}
	}
	//Clear flag timer4 
	TIMER4=0;
	T4CON &= ~(0x80);
}
*/


/* Converti le nombre de ticks (last_counter) en distance en cm*/
/*
unsigned int convert (void) {
	last_counter = (last_counter - 1);
	//Formule : distance (cm) = time(us)/58
	//On a une interruption toutes les 30us
	distance =(last_counter * 30)/58;
	return distance;
}*/

